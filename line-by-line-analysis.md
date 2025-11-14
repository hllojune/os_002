# 5. 코드 상세 분석 (Line-by-Line)

본 장에서는 `basic_interpreter.c`의 모든 코드를 기능 블록별로 나누어 상세히 분석합니다.

---

## 5.1. 헤더 파일 및 전처리기 (Lines 1-14)

```c
/* Basic Interpreter by Hüseyin Uslu raistlinthewiz@hotmail.com */
/* Code licenced under GPL */

#include <stdio.h>    // 파일 입출력 및 표준 입출력 함수 (fopen, fgets, printf 등)
#include <conio.h>    // 콘솔 입력 함수 (getch - 키 입력 대기)
#include <string.h>   // 문자열 처리 함수 (strcpy, strtok, strlen 등)
#include <stdlib.h>   // 메모리 할당 및 변환 함수 (malloc, free, atoi 등)
#include <ctype.h>    // 문자 분류 함수 (isdigit, isalpha, tolower 등)

#ifdef _WIN32
#define CLEAR() system("cls")      // Windows 환경: 화면 지우기
#else
#define CLEAR() system("clear")    // UNIX/Linux 환경: 화면 지우기
#endif
```

**설명:**
- GPL 라이센스로 배포되는 오픈소스 프로젝트입니다.
- 플랫폼별 조건부 컴파일을 통해 Windows와 UNIX 계열 운영체제 모두 지원합니다.

---

## 5.2. 자료구조 정의 (Lines 16-38)

### 5.2.1. Node 구조체 (Lines 16-23)

```c
struct node {
    int type;          // 노드 타입: 1=변수, 2=함수정의, 3=함수호출, 4=begin, 5=end
    char exp_data;     // 식별자 (변수명/함수명의 첫 글자)
    int val;           // 변수의 값 또는 연산 결과
    int line;          // 함수 정의 위치(line 번호) 또는 함수 호출 위치
    struct node* next; // 다음 노드를 가리키는 포인터 (연결 리스트)
};
typedef struct node Node;
```

**설명:**
- 인터프리터의 핵심 자료구조로, 스택의 각 항목을 표현합니다.
- **type 필드**가 노드의 역할을 결정합니다:
  - `1`: 변수 (예: `int a = 5`)
  - `2`: 함수 정의 (예: `function f(int x)`)
  - `3`: 함수 호출 마커 (복귀 주소 저장)
  - `4`: begin 블록 시작
  - `5`: end 블록 종료
- **exp_data**는 단일 문자만 저장하므로, 변수명/함수명이 한 글자로 제한됩니다.
- **line**은 함수 정의가 몇 번째 줄에 있는지, 또는 함수 호출이 어디서 일어났는지 기록합니다.

### 5.2.2. Stack 구조체 (Lines 25-26)

```c
struct stack { Node* top; };  // 스택의 최상단 노드를 가리키는 포인터
typedef struct stack Stack;
```

**설명:**
- LIFO(Last In First Out) 스택을 구현하기 위한 래퍼 구조체입니다.
- `top` 포인터를 통해 가장 최근에 push된 노드에 접근합니다.

### 5.2.3. OpStack 구조체 (Lines 28-32)

```c
struct opnode { 
    char op;               // 연산자 저장 (+, -, *, /)
    struct opnode* next;   // 다음 연산자 노드
};
typedef struct opnode opNode;

struct opstack { opNode* top; };  // 연산자 스택
typedef struct opstack OpStack;
```

**설명:**
- 중위 표기법(infix)을 후위 표기법(postfix)으로 변환할 때 사용하는 연산자 전용 스택입니다.
- 예: `(3 + 5)` → `3 5 +` 변환 시 `+` 연산자를 임시 저장

### 5.2.4. PostfixStack 구조체 (Lines 34-38)

```c
struct postfixnode { 
    int val;                       // 피연산자(숫자) 저장
    struct postfixnode* next;      // 다음 노드
};
typedef struct postfixnode Postfixnode;

struct postfixstack { Postfixnode* top; };  // 후위 표기법 계산용 스택
typedef struct postfixstack PostfixStack;
```

**설명:**
- 후위 표기법으로 변환된 수식을 계산할 때 피연산자를 저장하는 스택입니다.
- 예: `3 5 +` 계산 시 → 3 push → 5 push → `+` 만나면 pop 두 번 → 더한 결과 8 push

---

## 5.3. 함수 전방 선언 (Lines 40-44)

```c
static int GetVal(char, int*, Stack*);           // 변수/함수 값 조회
static int GetLastFunctionCall(Stack*);          // 가장 최근 함수 호출 위치 검색
static Stack* FreeAll(Stack*);                   // 스택 메모리 해제
static int my_stricmp(const char* a, const char* b);  // 대소문자 무시 문자열 비교
static void rstrip(char* s);                     // 문자열 끝 공백 제거
```

**설명:**
- `static` 키워드로 파일 내부에서만 사용되는 함수임을 명시합니다.
- 함수를 `main()` 앞에 선언하여 컴파일 순서 문제를 해결합니다.

---

## 5.4. 스택 연산 함수들

### 5.4.1. Push - 메인 스택에 노드 추가 (Lines 46-57)

```c
static Stack* Push(Node sNode, Stack* stck)
{
    // 1. 새 노드를 위한 메모리 동적 할당
    Node* newnode = (Node*)malloc(sizeof(Node));
    if (!newnode) { 
        printf("ERROR, Couldn't allocate memory..."); 
        return NULL;  // 메모리 할당 실패 시 NULL 반환
    }
    
    // 2. 매개변수로 받은 노드의 모든 필드를 새 노드에 복사
    newnode->type = sNode.type;
    newnode->val = sNode.val;
    newnode->exp_data = sNode.exp_data;
    newnode->line = sNode.line;
    
    // 3. 새 노드를 스택 최상단에 연결 (기존 top을 새 노드의 next로)
    newnode->next = stck->top;
    
    // 4. 스택의 top 포인터를 새 노드로 갱신
    stck->top = newnode;
    
    // 5. 메서드 체이닝을 위해 스택 포인터 반환
    return stck;
}
```

**핵심 동작:**
- 새로운 노드를 힙 메모리에 할당하고, 스택의 최상단에 삽입합니다.
- 스택은 항상 `top`에서 확장하므로, 가장 최근 항목이 먼저 조회됩니다.

### 5.4.2. PushOp - 연산자 스택에 추가 (Lines 59-67)

```c
static OpStack* PushOp(char op, OpStack* opstck)
{
    opNode* newnode = (opNode*)malloc(sizeof(opNode));
    if (!newnode) { 
        printf("ERROR, Couldn't allocate memory..."); 
        return NULL; 
    }
    newnode->op = op;              // 연산자 저장 (+, -, *, /)
    newnode->next = opstck->top;   // 기존 top을 next로 연결
    opstck->top = newnode;         // top 갱신
    return opstck;
}
```

### 5.4.3. PopOp - 연산자 스택에서 제거 (Lines 69-82)

```c
static char PopOp(OpStack* opstck)
{
    opNode* temp;
    char op;
    
    // 1. 스택이 비어있는지 확인
    if (opstck->top == NULL)
    {
        return 0;  // NUL 문자 반환 (스택 비어있음을 의미)
    }
    
    // 2. 최상단 노드의 연산자 추출
    op = opstck->top->op;
    
    // 3. 최상단 노드 포인터를 임시 저장
    temp = opstck->top;
    
    // 4. top을 다음 노드로 이동
    opstck->top = opstck->top->next;
    
    // 5. 제거된 노드의 메모리 해제
    free(temp);
    
    // 6. 추출한 연산자 반환
    return op;
}
```

### 5.4.4. PushPostfix / PopPostfix (Lines 84-107)

```c
// PushPostfix: 후위 표기법 계산 시 피연산자를 스택에 추가
static PostfixStack* PushPostfix(int val, PostfixStack* poststck)
{
    Postfixnode* newnode = (Postfixnode*)malloc(sizeof(Postfixnode));
    if (!newnode) { 
        printf("ERROR, Couldn't allocate memory..."); 
        return NULL; 
    }
    newnode->val = val;            // 정수 값 저장
    newnode->next = poststck->top;
    poststck->top = newnode;
    return poststck;
}

// PopPostfix: 후위 표기법 계산 시 피연산자를 스택에서 꺼냄
static int PopPostfix(PostfixStack* poststck)
{
    Postfixnode* temp;
    int val;
    
    if (poststck->top == NULL)
    {
        return 0;  // 스택 비어있으면 0 반환
    }
    
    val = poststck->top->val;      // 최상단 값 추출
    temp = poststck->top;
    poststck->top = poststck->top->next;
    free(temp);                    // 메모리 해제
    return val;
}
```

### 5.4.5. Pop - 메인 스택에서 제거 (Lines 109-120)

```c
static void Pop(Node* sNode, Stack* stck)
{
    Node* temp;
    
    // 1. 스택이 비어있으면 아무 작업도 하지 않음
    if (stck->top == NULL) return;
    
    // 2. 최상단 노드의 모든 필드를 매개변수로 받은 노드에 복사
    sNode->exp_data = stck->top->exp_data;
    sNode->type = stck->top->type;
    sNode->line = stck->top->line;
    sNode->val = stck->top->val;
    
    // 3. 최상단 노드를 임시 저장
    temp = stck->top;
    
    // 4. top을 다음 노드로 이동
    stck->top = stck->top->next;
    
    // 5. 제거된 노드의 메모리 해제
    free(temp);
}
```

**주의사항:**
- 이 함수는 값을 반환하지 않고, 매개변수로 받은 `sNode`에 값을 복사합니다.
- 반드시 호출 전에 `sNode`를 선언해두어야 합니다.

---

## 5.5. 유틸리티 함수들

### 5.5.1. isStackEmpty (Lines 122-125)

```c
static int isStackEmpty(OpStack* stck)
{
    return stck->top == 0;  // top이 NULL이면 1(true), 아니면 0(false) 반환
}
```

**설명:**
- 연산자 스택이 비어있는지 확인하는 불린 함수입니다.
- 후위 표기법 변환 중 스택을 pop하기 전에 반드시 확인해야 합니다.

### 5.5.2. Priotry - 연산자 우선순위 (Lines 127-132)

```c
static int Priotry(char operator)
{
    if ((operator=='+') || (operator=='-')) 
        return 1;  // 덧셈/뺄셈: 낮은 우선순위
    else if ((operator=='/') || (operator=='*')) 
        return 2;  // 곱셈/나눗셈: 높은 우선순위
    return 0;      // 기타: 우선순위 없음
}
```

**설명:**
- 중위 표기법을 후위 표기법으로 변환할 때 연산자 우선순위를 판단합니다.
- 곱셈/나눗셈(2)이 덧셈/뺄셈(1)보다 높은 우선순위를 가집니다.

---

## 5.6. main 함수 (Lines 134-446)

### 5.6.1. 변수 선언 및 초기화 (Lines 136-168)

```c
int main(int argc, char** argv)
{
    // ===== 버퍼 변수들 =====
    char line[4096];          // 파일에서 읽은 현재 라인 저장
    char dummy[4096];         // 파일 포인터 이동 시 스킵용 버퍼
    char lineyedek[4096];     // 원본 라인 백업 (토큰화 시 원본 손상 방지)
    char postfix[4096];       // 후위 표기법 변환 결과 저장
    char* firstword;          // strtok()로 추출한 첫 번째 단어 포인터

    // ===== 후위 표기법 계산용 변수 =====
    int val1;                 // 첫 번째 피연산자
    int val2;                 // 두 번째 피연산자

    // ===== 실행 상태 추적 변수 =====
    int LastExpReturn = 0;         // 마지막으로 계산된 수식의 결과
    int LastFunctionReturn = -999; // 함수 반환 값 (-999는 아직 반환 없음을 의미)
    int CalingFunctionArgVal = 0;  // 함수 호출 시 전달되는 인자 값

    // ===== 재사용 가능한 임시 노드 =====
    Node tempNode;

    // ===== 스택 동적 할당 =====
    OpStack* MathStack = (OpStack*)malloc(sizeof(OpStack));        // 연산자 스택
    FILE* filePtr;                                                  // 파일 포인터
    PostfixStack* CalcStack = (PostfixStack*)malloc(sizeof(PostfixStack)); // 후위 계산 스택
    int resultVal = 0;                                              // 연산 결과 임시 저장
    Stack* STACK = (Stack*)malloc(sizeof(Stack));                  // 메인 스택

    // ===== 실행 제어 변수 =====
    int curLine = 0;          // 현재 읽고 있는 소스 파일의 라인 번호
    int foundMain = 0;        // main 함수를 찾았는지 여부 (0=false, 1=true)
    int WillBreak = 0;        // 함수 호출로 인해 루프를 중단해야 하는지 여부

    // ===== 메모리 할당 실패 검사 =====
    if (!MathStack || !CalcStack || !STACK) {
        printf("Memory alloc failed\n");
        return 1;
    }
    
    // ===== 스택 초기화 (모든 top을 NULL로) =====
    MathStack->top = NULL;
    CalcStack->top = NULL;
    STACK->top = NULL;

    CLEAR();  // 콘솔 화면 지우기
```

**핵심 설명:**
- **LastFunctionReturn = -999**: 센티널 값으로, 함수가 아직 반환되지 않았음을 표시합니다.
- **foundMain 플래그**: `function main()`을 만나기 전까지는 코드를 실행하지 않습니다.
- **WillBreak 플래그**: 함수 호출이 발생하면 현재 수식 파싱을 중단하고, 파일 포인터를 함수 정의로 이동시킵니다.

### 5.6.2. 명령행 인자 검증 및 파일 열기 (Lines 171-183)

```c
    // ===== 명령행 인자 개수 확인 =====
    if (argc != 2)  // 프로그램명 + 파일명 = 2개 인자 필요
    {
        printf("Incorrect arguments!\n");
        printf("Usage: %s <inputfile.spl>", argv[0]);  // 사용법 출력
        return 1;  // 에러 코드 1 반환
    }

    // ===== 소스 파일 열기 =====
    filePtr = fopen(argv[1], "r");  // 읽기 모드로 파일 오픈
    if (filePtr == NULL)
    {
        printf("Can't open %s. Check the file please", argv[1]);
        return 2;  // 에러 코드 2 반환 (파일 열기 실패)
    }
```

**설명:**
- 프로그램 실행 방법: `./basic_interpreter input1.spl`
- `argc`가 2가 아니면 (인자 부족/과다) 사용법을 출력하고 종료합니다.
- 파일 열기 실패 시 에러 메시지를 출력하고 프로그램을 종료합니다.

### 5.6.3. 메인 루프 - 파일 라인별 처리 (Lines 185-438)

```c
    // ===== 파일을 한 줄씩 읽는 메인 루프 =====
    while (fgets(line, 4096, filePtr))  // EOF까지 반복
    {
        int k = 0;

        // ----- 1단계: 탭 문자를 공백으로 변환 -----
        while (line[k] != '\0')
        {
            if (line[k] == '\t') 
                line[k] = ' ';  // 일관된 토큰화를 위해 탭을 공백으로 치환
            k++;
        }

        // ----- 2단계: 라인 끝 공백/개행 문자 제거 -----
        rstrip(line);
        
        // ----- 3단계: 원본 라인 백업 (strtok은 원본을 수정하므로) -----
        strcpy(lineyedek, line);

        // ----- 4단계: 라인 번호 증가 및 tempNode 초기화 -----
        curLine++;
        tempNode.val = -999;        // 센티널 값
        tempNode.exp_data = ' ';    // 공백으로 초기화
        tempNode.line = -999;
        tempNode.type = -999;
```

**주요 전처리 과정:**
1. **탭 정규화**: 탭 문자를 공백으로 변환하여 일관된 토큰 분리
2. **공백 제거**: 라인 끝의 `\n`, `\r`, 공백 제거
3. **백업 생성**: 수식 파싱 시 원본이 필요하므로 `lineyedek`에 복사
4. **노드 초기화**: 재사용되는 `tempNode`를 센티널 값으로 리셋

---

## 5.7. begin/end 처리 (Lines 204-247)

```c
        // ===== "begin" 키워드 처리 =====
        if (my_stricmp("begin", line) == 0)  // 대소문자 무시 비교
        {
            if (foundMain)  // main 함수 안에서만 처리
            {
                tempNode.type = 4;  // type 4 = begin 블록 시작
                STACK = Push(tempNode, STACK);  // 스택에 마커 push
            }
        }
```

**설명:**
- `begin`은 블록의 시작을 표시하는 마커입니다.
- **중요**: `foundMain`이 1일 때만 스택에 push합니다.
  - 이유: main 함수를 찾기 전까지는 다른 함수들의 begin을 무시해야 합니다.

```c
        // ===== "end" 키워드 처리 =====
        else if (my_stricmp("end", line) == 0)
        {
            if (foundMain)
            {
                int sline;
                tempNode.type = 5;  // type 5 = end 블록 종료
                STACK = Push(tempNode, STACK);

                // ----- 최근 함수 호출 위치 검색 -----
                sline = GetLastFunctionCall(STACK);
                
                if (sline == 0)  // 함수 호출이 없음 = main 함수 종료
                {
                    // ===== 최종 결과 출력 =====
                    printf("Output=%d", LastExpReturn);
                }
                else  // 함수 호출이 있음 = 함수에서 반환
                {
                    int j;
                    int foundCall = 0;
                    
                    // 1. 함수 반환 값 저장
                    LastFunctionReturn = LastExpReturn;

                    // 2. 파일 포인터를 호출 지점으로 되돌리기
                    fclose(filePtr);  // 현재 파일 닫기
                    filePtr = fopen(argv[1], "r");  // 다시 열기 (처음부터)
                    curLine = 0;
                    
                    // 3. 호출 지점(sline)까지 라인 스킵
                    for (j = 1; j < sline; j++)
                    {
                        fgets(dummy, 4096, filePtr);  // 읽고 버리기
                        curLine++;
                    }

                    // 4. 스택에서 함수 관련 노드들을 모두 pop
                    while (foundCall == 0)
                    {
                        Pop(&tempNode, STACK);
                        if (tempNode.type == 3)  // type 3 = 함수 호출 마커
                            foundCall = 1;  // 호출 마커를 찾으면 종료
                    }
                }
            }
        }
```

**end 처리의 두 가지 경로:**

**경로 1: main 함수 종료** (`sline == 0`)
- GetLastFunctionCall()이 0을 반환 = 스택에 함수 호출 마커(type 3)가 없음
- 프로그램 최종 결과를 `Output=값` 형식으로 출력

**경로 2: 서브 함수 종료** (`sline != 0`)
1. `LastFunctionReturn`에 함수 결과 저장
2. 파일을 닫고 다시 열어 **파일 포인터를 처음으로 되돌림**
3. `sline`(함수 호출이 일어난 라인)까지 라인을 스킵
4. 스택에서 함수의 지역변수, begin/end 마커, 함수 호출 마커(type 3)를 모두 pop

**핵심 메커니즘:**
- 이 인터프리터는 **파일 포인터 조작**을 통해 함수 호출과 복귀를 구현합니다.
- 재귀나 AST를 사용하지 않고, 파일을 여러 번 읽어가며 실행합니다.

---

## 5.8. 변수 선언 처리 (Lines 253-275)

```c
        else  // begin/end가 아닌 경우
        {
            // ===== 첫 번째 단어 추출 (공백 기준) =====
            firstword = strtok(line, " ");
            if (!firstword) continue;  // 빈 라인이면 스킵

            // ===== "int" 키워드 처리 (변수 선언) =====
            if (my_stricmp("int", firstword) == 0)
            {
                if (foundMain)  // main 발견 후에만 처리
                {
                    tempNode.type = 1;  // type 1 = 변수
                    
                    // ----- 변수명 추출 -----
                    firstword = strtok(NULL, " ");  // 다음 토큰 (변수명)
                    if (!firstword) continue;
                    tempNode.exp_data = firstword[0];  // 첫 글자만 식별자로 저장
                    
                    // ----- '=' 또는 값 추출 -----
                    firstword = strtok(NULL, " ");  // 다음 토큰 ('=' 또는 값)
                    if (!firstword) continue;
                    
                    // "=" 기호가 있으면 한 번 더 토큰 추출
                    if (my_stricmp("=", firstword) == 0)
                    {
                        firstword = strtok(NULL, " ");  // 실제 값
                        if (!firstword) continue;
                    }

                    // ----- 값 변환 및 스택에 push -----
                    tempNode.val = atoi(firstword);  // 문자열 → 정수 변환
                    tempNode.line = 0;  // 변수는 라인 번호 불필요
                    STACK = Push(tempNode, STACK);  // 스택에 추가
                }
            }
```

**파싱 예시:**

입력: `int a = 5;`
1. `firstword = "int"` → 변수 선언 인식
2. `firstword = "a"` → 변수명 추출 (`exp_data = 'a'`)
3. `firstword = "="` → 등호 발견
4. `firstword = "5"` → 값 추출
5. `atoi("5")` → 정수 5로 변환
6. 노드 `{type:1, exp_data:'a', val:5, line:0}`를 스택에 push

**제한사항:**
- 변수명이 **한 글자**만 지원됩니다 (`exp_data`가 char 타입).
- `int abc = 10;`을 선언해도 `'a'`만 저장됩니다.

---

## 5.9. 함수 정의 처리 (Lines 276-304)

```c
            // ===== "function" 키워드 처리 =====
            else if (my_stricmp("function", firstword) == 0)
            {
                // ----- 함수명 추출 -----
                firstword = strtok(NULL, " ");  // 다음 토큰 (함수명)
                if (!firstword) continue;

                // ----- 함수 정의 노드 생성 -----
                tempNode.type = 2;  // type 2 = 함수 정의
                tempNode.exp_data = firstword[0];  // 함수명의 첫 글자
                tempNode.line = curLine;  // 함수 정의가 있는 라인 번호 저장
                tempNode.val = 0;  // 함수는 값이 없음
                STACK = Push(tempNode, STACK);  // 스택에 등록

                // ----- "main" 함수인지 확인 -----
                if (firstword[0] == 'm' && firstword[1] == 'a' && 
                    firstword[2] == 'i' && firstword[3] == 'n')
                {
                    foundMain = 1;  // main 함수 발견 플래그 설정
                }
                else  // main이 아닌 다른 함수
                {
                    if (foundMain)  // main 실행 중에 다른 함수 정의를 만난 경우
                    {
                        // ----- 함수 매개변수 처리 -----
                        firstword = strtok(NULL, " ");  // "int" 또는 매개변수명
                        if (!firstword) continue;
                        
                        // 매개변수를 변수처럼 스택에 추가
                        tempNode.type = 1;  // type 1 = 변수
                        tempNode.exp_data = firstword[0];  // 매개변수명 첫 글자
                        tempNode.val = CalingFunctionArgVal;  // 호출 시 전달된 인자 값
                        tempNode.line = 0;
                        STACK = Push(tempNode, STACK);
                    }
                }
            }
```

**함수 정의 처리 흐름:**

**케이스 1: main 함수 정의**
```
입력: function main()

1. tempNode.type = 2 (함수 정의)
2. tempNode.exp_data = 'm'
3. tempNode.line = 현재 라인 번호
4. 스택에 push
5. foundMain = 1 설정 → 이제부터 코드 실행 시작
```

**케이스 2: 일반 함수 정의 (main 발견 전)**
```
입력: function f(int a)

1. 함수 정의 노드를 스택에 push
2. foundMain == 0이므로 매개변수는 처리하지 않음
3. 함수 정의만 등록하고 넘어감
```

**케이스 3: 함수 호출 중 함수 정의 만남**
```
상황: main에서 f(4)를 호출하여 파일을 되감아 f 정의로 이동

1. 함수 정의 노드 push (이미 있지만 다시 push됨)
2. foundMain == 1이므로 매개변수 처리:
   - CalingFunctionArgVal에 저장된 인자 값(4)을
   - 매개변수 'a'의 값으로 설정
   - type 1 노드로 스택에 push
3. 이제 f 함수 내부의 begin ~ end를 실행
```

**중요한 설계:**
- 함수 정의(type 2)는 **함수가 몇 번째 줄에 있는지** 저장합니다.
- GetVal() 함수가 함수명을 찾으면, 해당 라인 번호를 반환합니다.
- 파일 포인터를 그 라인으로 이동시켜 함수를 "실행"합니다.

---

## 5.10. 수식 처리 - 중위→후위 변환 (Lines 305-394)

```c
            // ===== 수식 라인 처리 (괄호로 시작하는 라인) =====
            else if (firstword[0] == '(')
            {
                if (foundMain)  // main 발견 후에만 수식 계산
                {
                    int i = 0;  // lineyedek 인덱스
                    int y = 0;  // postfix 버퍼 인덱스

                    MathStack->top = NULL;  // 연산자 스택 초기화

                    // ===== 1단계: 중위 표기법 → 후위 표기법 변환 =====
                    while (lineyedek[i] != '\0')  // 문자열 끝까지 반복
                    {
                        // ----- Case 1: 숫자(피연산자) -----
                        if (isdigit((unsigned char)lineyedek[i]))
                        {
                            postfix[y] = lineyedek[i];  // 숫자는 바로 후위식에 추가
                            y++;
                        }
                        
                        // ----- Case 2: 닫는 괄호 ')' -----
                        else if (lineyedek[i] == ')')
                        {
                            // 여는 괄호를 만날 때까지 스택에서 연산자 pop
                            if (!isStackEmpty(MathStack))
                            {
                                postfix[y] = PopOp(MathStack);  // 연산자 추출
                                y++;
                            }
                        }
                        
                        // ----- Case 3: 연산자 (+, -, *, /) -----
                        else if (lineyedek[i] == '+' || lineyedek[i] == '-' || 
                                 lineyedek[i] == '*' || lineyedek[i] == '/')
                        {
                            if (isStackEmpty(MathStack))  // 스택이 비어있으면
                            {
                                MathStack = PushOp(lineyedek[i], MathStack);  // 그냥 push
                            }
                            else  // 스택에 연산자가 이미 있으면
                            {
                                // 우선순위 비교
                                if (Priotry(lineyedek[i]) <= Priotry(MathStack->top->op))
                                {
                                    // 현재 연산자의 우선순위가 낮거나 같으면
                                    // 스택의 연산자를 먼저 pop하여 후위식에 추가
                                    postfix[y] = PopOp(MathStack);
                                    y++;
                                    MathStack = PushOp(lineyedek[i], MathStack);
                                }
                                else  // 현재 연산자의 우선순위가 높으면
                                {
                                    MathStack = PushOp(lineyedek[i], MathStack);  // 그냥 push
                                }
                            }
                        }
```

**중위→후위 변환 알고리즘 (Shunting Yard Algorithm):**

**예시 1: `(3 + 5)`**
```
1. '(' → 무시
2. '3' → postfix에 추가: "3"
3. '+' → 스택 비어있음 → push
4. '5' → postfix에 추가: "35"
5. ')' → 스택에서 pop → postfix: "35+"
```

**예시 2: `(2 * 3 + 4)`**
```
1. '2' → postfix: "2"
2. '*' → push (스택: *)
3. '3' → postfix: "23"
4. '+' → 우선순위 비교:
   - '+'의 우선순위(1) < '*'의 우선순위(2)
   - '*'를 pop하여 postfix에 추가: "23*"
   - '+'를 push (스택: +)
5. '4' → postfix: "23*4"
6. ')' → '+'를 pop: "23*4+"
```

**제한사항:**
- **한 자리 숫자**만 처리 가능 (`isdigit()`이 문자 단위로 검사)
- 다자리 숫자(예: 123)는 '1', '2', '3'으로 각각 처리되어 오류 발생

```c
                        // ----- Case 4: 알파벳(변수 또는 함수 호출) -----
                        else if (isalpha((unsigned char)lineyedek[i]) > 0)
                        {
                            int codeline = 0;  // 함수 정의 라인 번호
                            int dummyint = 0;  // 사용하지 않는 더미 변수
                            
                            // GetVal로 변수/함수 조회
                            int retVal = GetVal(lineyedek[i], &codeline, STACK);

                            // ----- 변수인 경우 -----
                            if ((retVal != -1) && (retVal != -999))
                            {
                                // 변수의 값을 ASCII 숫자로 변환하여 추가
                                postfix[y] = (char)(retVal + 48);  // 48 = '0'의 ASCII
                                y++;
                            }
                            // ----- 함수 호출 또는 미정의 식별자 -----
                            else
                            {
                                // 아직 함수 반환 값이 없는 경우 (첫 호출)
                                if (LastFunctionReturn == -999)
                                {
                                    int j;
                                    
                                    // 1. 함수 호출 마커를 스택에 push
                                    tempNode.type = 3;  // type 3 = 함수 호출
                                    tempNode.line = curLine;  // 현재 라인 저장 (복귀 주소)
                                    STACK = Push(tempNode, STACK);

                                    // 2. 함수 인자 값 추출 (예: f(c)에서 c의 값)
                                    // lineyedek[i+2]는 함수명 다음 2칸 뒤의 문자 (인자)
                                    // 예: "f(c)" → i='f', i+2='c'
                                    CalingFunctionArgVal = GetVal(lineyedek[i + 2], &dummyint, STACK);

                                    // 3. 파일 포인터를 함수 정의 위치로 이동
                                    fclose(filePtr);
                                    filePtr = fopen(argv[1], "r");
                                    curLine = 0;

                                    // 4. codeline(함수 정의 라인)까지 스킵
                                    for (j = 1; j < codeline; j++)
                                    {
                                        fgets(dummy, 4096, filePtr);
                                        curLine++;
                                    }

                                    // 5. 수식 파싱 중단 플래그 설정
                                    WillBreak = 1;
                                    break;  // while 루프 탈출
                                }
                                // 함수가 이미 반환된 경우 (재개)
                                else
                                {
                                    // 함수 반환 값을 후위식에 추가
                                    postfix[y] = (char)(LastFunctionReturn + 48);
                                    y++;
                                    i = i + 3;  // "f(c)" 전체를 건너뜀 (3칸 이동)
                                    LastFunctionReturn = -999;  // 센티널 값으로 리셋
                                }
                            }
                        }
                        i++;  // 다음 문자로 이동
                    }  // while (lineyedek[i] != '\0') 끝
```

**함수 호출 메커니즘:**

**상황: `((6 + f(c)) / b)` 수식을 만남**

1. `'6'` → postfix: "6"
2. `'+'` → push
3. `'f'` → 알파벳 발견
   - GetVal('f', &codeline, STACK) 호출
   - 반환값: -1 (함수는 type 2이므로)
   - codeline에 함수 정의 라인 저장됨
4. LastFunctionReturn이 -999이므로 **첫 호출**:
   - type 3 노드 push (복귀 주소 저장)
   - `lineyedek[i+2]`의 값('c') 조회 → CalingFunctionArgVal에 저장
   - 파일을 닫고 다시 열어 함수 정의로 이동
   - WillBreak = 1 설정 후 break
5. 다음 루프에서 f 함수의 begin ~ end를 실행
6. f 함수가 end를 만나면:
   - LastFunctionReturn에 결과 저장
   - 파일 포인터를 원래 위치로 복귀
   - 스택에서 f의 변수들 pop
7. 다시 `((6 + f(c)) / b)` 라인을 만남
8. `'f'` → LastFunctionReturn != -999이므로 **재개**:
   - 반환 값을 postfix에 추가
   - i를 3칸 이동 (f(c) 건너뜀)
9. 나머지 수식 파싱 계속

---

## 5.11. 수식 처리 - 후위 표기법 계산 (Lines 396-434)

```c
                    // ===== 2단계: 후위 표기법 계산 (WillBreak가 0일 때만) =====
                    if (WillBreak == 0)  // 함수 호출로 중단되지 않았으면
                    {
                        // ----- 남은 연산자들을 모두 pop하여 후위식에 추가 -----
                        while (!isStackEmpty(MathStack))
                        {
                            postfix[y] = PopOp(MathStack);
                            y++;
                        }

                        postfix[y] = '\0';  // 문자열 종료 문자 추가

                        // ----- 후위 표기법 계산 시작 -----
                        i = 0;
                        CalcStack->top = NULL;  // 계산 스택 초기화
                        
                        while (postfix[i] != '\0')
                        {
                            // ----- Case 1: 숫자(피연산자) -----
                            if (isdigit((unsigned char)postfix[i]))
                            {
                                // ASCII 숫자를 정수로 변환하여 push
                                // '3' - '0' = 3
                                CalcStack = PushPostfix(postfix[i] - '0', CalcStack);
                            }
                            // ----- Case 2: 연산자 -----
                            else if (postfix[i] == '+' || postfix[i] == '-' || 
                                     postfix[i] == '*' || postfix[i] == '/')
                            {
                                // 피연산자 2개를 pop (순서 주의!)
                                val1 = PopPostfix(CalcStack);  // 두 번째 피연산자
                                val2 = PopPostfix(CalcStack);  // 첫 번째 피연산자

                                // 연산 수행
                                switch (postfix[i])
                                {
                                case '+': resultVal = val2 + val1; break;
                                case '-': resultVal = val2 - val1; break;
                                case '/': resultVal = val2 / val1; break;  // 정수 나눗셈
                                case '*': resultVal = val2 * val1; break;
                                }
                                
                                // 결과를 다시 스택에 push
                                CalcStack = PushPostfix(resultVal, CalcStack);
                            }
                            i++;
                        }

                        // ----- 최종 결과 저장 -----
                        LastExpReturn = CalcStack->top->val;
                    }
                    WillBreak = 0;  // 플래그 리셋
                }
            }
        }
    }  // while (fgets(...)) 끝
```

**후위 표기법 계산 알고리즘:**

**예시: `35+` (3 + 5)**
```
1. '3' → push(3)    스택: [3]
2. '5' → push(5)    스택: [3, 5]
3. '+' → pop 2개:
   - val1 = 5 (두 번째 피연산자)
   - val2 = 3 (첫 번째 피연산자)
   - 계산: 3 + 5 = 8
   - push(8)        스택: [8]
4. 최종 결과: 8
```

**예시: `23*4+` (2 * 3 + 4)**
```
1. '2' → push(2)    스택: [2]
2. '3' → push(3)    스택: [2, 3]
3. '*' → pop 2개:
   - val1 = 3, val2 = 2
   - 2 * 3 = 6
   - push(6)        스택: [6]
4. '4' → push(4)    스택: [6, 4]
5. '+' → pop 2개:
   - val1 = 4, val2 = 6
   - 6 + 4 = 10
   - push(10)       스택: [10]
6. 최종 결과: 10
```

**순서 주의사항:**
```c
val1 = PopPostfix(CalcStack);  // 나중에 push된 값
val2 = PopPostfix(CalcStack);  // 먼저 push된 값
resultVal = val2 - val1;       // val2가 피감수, val1이 감수
```
- 뺄셈과 나눗셈은 **순서가 중요**합니다.
- 스택에서 pop하는 순서를 반대로 해야 올바른 계산이 됩니다.

---

## 5.12. 프로그램 종료 처리 (Lines 440-446)

```c
    // ===== 파일 닫기 =====
    fclose(filePtr);
    
    // ===== 스택 메모리 해제 =====
    STACK = FreeAll(STACK);

    // ===== 종료 메시지 및 키 입력 대기 =====
    printf("\nPress a key to exit...");
    getch();  // 사용자가 키를 누를 때까지 대기
    
    return 0;  // 정상 종료
}
```

**메모리 관리:**
- FreeAll()을 호출하여 스택의 모든 노드를 해제합니다.
- MathStack과 CalcStack은 해제하지 않는데, 이는 **메모리 누수**입니다.
  - 개선: `free(MathStack); free(CalcStack);` 추가 필요

---

## 5.13. 보조 함수들 (Lines 448-503)

### 5.13.1. FreeAll - 스택 메모리 해제 (Lines 448-457)

```c
static Stack* FreeAll(Stack* stck)
{
    Node* head = stck->top;  // 스택의 최상단부터 시작
    
    // 모든 노드를 순회하며 메모리 해제
    while (head) {
        Node* temp = head;       // 현재 노드 저장
        head = head->next;       // 다음 노드로 이동
        free(temp);              // 현재 노드 메모리 해제
    }
    
    stck->top = NULL;  // top 포인터 초기화
    return NULL;       // NULL 반환 (사용되지 않음)
}
```

**동작 원리:**
- 연결 리스트의 모든 노드를 순회하며 free()를 호출합니다.
- temp 변수를 사용하여 현재 노드를 저장한 후, head를 다음으로 이동시킵니다.
- 이렇게 하지 않으면 `head = head->next` 실행 후 free(head)를 호출할 수 없습니다.

### 5.13.2. GetLastFunctionCall - 함수 호출 위치 검색 (Lines 459-467)

```c
static int GetLastFunctionCall(Stack* stck)
{
    Node* head = stck->top;  // 스택 최상단부터 검색
    
    while (head) {
        // type 3인 노드(함수 호출 마커)를 찾으면
        if (head->type == 3) 
            return head->line;  // 호출이 일어난 라인 번호 반환
        head = head->next;      // 다음 노드로 이동
    }
    
    return 0;  // 함수 호출이 없으면 0 반환
}
```

**사용 시나리오:**
- end를 만났을 때, 이것이 함수의 끝인지 main의 끝인지 판단합니다.
- 스택에 type 3 노드가 있으면 → 함수에서 호출자로 복귀해야 함
- 스택에 type 3 노드가 없으면 → main 함수 종료 (프로그램 끝)

### 5.13.3. GetVal - 변수/함수 값 조회 (Lines 469-484)

```c
static int GetVal(char exp_name, int* line, Stack* stck)
{
    Node* head;
    *line = 0;  // 출력 매개변수 초기화
    
    // 스택이 비어있으면 -999 반환 (식별자 없음)
    if (stck->top == NULL) 
        return -999;
    
    head = stck->top;  // 스택 최상단부터 검색
    
    while (head) {
        // 식별자가 일치하는 노드를 찾으면
        if (head->exp_data == exp_name)
        {
            // type 1 (변수): 값 반환
            if (head->type == 1) 
                return head->val;
            
            // type 2 (함수 정의): 라인 번호 저장 후 -1 반환
            else if (head->type == 2) { 
                *line = head->line; 
                return -1; 
            }
        }
        head = head->next;  // 다음 노드로 이동
    }
    
    return -999;  // 식별자를 찾지 못함
}
```

**반환 값의 의미:**
- **정수 값 (0~9)**: 변수를 찾았고, 그 값을 반환
- **-1**: 함수를 찾았음. `*line`에 함수 정의 라인 저장됨
- **-999**: 식별자를 찾지 못함 (미정의 변수/함수)

**중요한 동작:**
- 스택을 **top부터** 검색하므로, 가장 최근에 선언된 변수를 먼저 찾습니다.
- 이것이 **변수 스코프(scope)** 를 구현하는 방법입니다.

**예시:**
```c
function main()
begin
   int a = 1;      // [3] 두 번째 a
   function f(int a)  // 함수 호출로 f로 진입
   begin
      int a = 4;   // [1] Stack top (가장 최근)
      int b = 6;   // [2]
      ...
```
- GetVal('a')를 호출하면 [1]의 a (값 4)를 반환합니다.
- f 함수가 종료되고 스택에서 pop되면, GetVal('a')는 [3]의 a (값 1)를 반환합니다.

### 5.13.4. my_stricmp - 대소문자 무시 문자열 비교 (Lines 488-496)

```c
static int my_stricmp(const char* a, const char* b)
{
    unsigned char ca, cb;
    
    // 두 문자열이 모두 끝날 때까지 반복
    while (*a || *b) {
        ca = (unsigned char)tolower((unsigned char)*a);  // 소문자로 변환
        cb = (unsigned char)tolower((unsigned char)*b);
        
        // 문자가 다르면 차이 반환
        if (ca != cb) 
            return (int)ca - (int)cb;
        
        // 다음 문자로 이동
        if (*a) a++;
        if (*b) b++;
    }
    
    return 0;  // 두 문자열이 같음
}
```

**사용 이유:**
- 표준 라이브러리의 `stricmp()` 또는 `strcasecmp()`가 플랫폼마다 다르므로
- 직접 구현하여 이식성을 높였습니다.
- "BEGIN", "begin", "BeGiN" 모두 같은 키워드로 인식합니다.

### 5.13.5. rstrip - 문자열 끝 공백 제거 (Lines 498-503)

```c
static void rstrip(char* s)
{
    size_t n = strlen(s);  // 문자열 길이 계산
    
    // 문자열 끝에서부터 개행/공백을 제거
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' ')) 
        s[--n] = '\0';  // 해당 위치를 null 문자로 치환
}
```

**처리하는 문자:**
- `\n` (Line Feed) - UNIX/Linux 개행
- `\r` (Carriage Return) - Windows 개행의 일부
- ` ` (공백)

**동작 방식:**
- 문자열 끝에서부터 역방향으로 검사하며, 공백 문자를 `\0`으로 치환합니다.
- 공백이 아닌 문자를 만나면 루프를 종료합니다.

---

## 5.14. 전체 실행 흐름 요약

### 입력: input1.spl

```
function f(int a)
begin
   int b = 6;
   int c = 2;
   ((b+c)/a);
end

function main()
begin
   int a = 1;
   int b = 2;
   int c = 4;
   ((6 + f(c) ) / b);
end
```

### 실행 과정:

**Phase 1: 함수 정의 수집**
```
Line 1: function f(int a)
  → type 2 노드 push: {type:2, exp_data:'f', line:1}
  → foundMain은 여전히 0 (실행 안 함)

Line 8: function main()
  → type 2 노드 push: {type:2, exp_data:'m', line:8}
  → foundMain = 1 설정
```

**Phase 2: main 함수 실행**
```
Line 9: begin
  → type 4 노드 push

Line 10-12: 변수 선언
  → type 1 노드 3개 push: a=1, b=2, c=4

Line 13: ((6 + f(c) ) / b);
  → 수식 파싱 시작
  → '6' → postfix: "6"
  → '+' → MathStack에 push
  → 'f' → 알파벳 발견
    ✓ GetVal('f') → codeline=1, 반환값=-1 (함수)
    ✓ type 3 노드 push: {type:3, line:13}
    ✓ GetVal('c') → CalingFunctionArgVal=4
    ✓ 파일을 line 1로 되감기
    ✓ WillBreak=1, break
```

**Phase 3: f 함수 실행**
```
Line 1: function f(int a) (재진입)
  → type 2 노드 push (중복)
  → foundMain=1이므로 매개변수 처리:
    ✓ type 1 노드 push: {type:1, exp_data:'a', val:4}
    (CalingFunctionArgVal=4를 사용)

Line 2: begin
  → type 4 노드 push

Line 3-4: 변수 선언
  → type 1 노드 push: b=6
  → type 1 노드 push: c=2

Line 5: ((b+c)/a);
  → 중위→후위 변환: "bc+a/" 
  → GetVal('b') → 6 (f의 지역변수)
  → GetVal('c') → 2 (f의 지역변수)
  → GetVal('a') → 4 (f의 매개변수)
  → 후위 계산: 6 2 + 4 / = 8 / 4 = 2
  → LastExpReturn = 2

Line 6: end
  → GetLastFunctionCall() → 13 반환
  → LastFunctionReturn = 2 저장
  → 파일을 line 13으로 되감기
  → 스택 정리: f의 변수들 모두 pop
```

**Phase 4: main 함수 복귀 및 최종 계산**
```
Line 13: ((6 + f(c) ) / b); (재진입)
  → 수식 파싱 재개
  → '6' → postfix: "6"
  → '+' → MathStack에 push
  → 'f' → 알파벳 발견
    ✓ LastFunctionReturn != -999 (이미 반환됨)
    ✓ postfix에 추가: "62" (2는 함수 반환값)
    ✓ i += 3으로 f(c) 건너뜀
  → ')' → '+' pop → postfix: "62+"
  → '/' → MathStack에 push
  → 'b' → GetVal('b') → 2 (main의 지역변수)
  → postfix에 추가: "62+2"
  → ')' → '/' pop → postfix: "62+2/"
  
  → 후위 계산:
    ✓ 6 push
    ✓ 2 push
    ✓ '+' → 6+2=8 push
    ✓ 2 push
    ✓ '/' → 8/2=4 push
  → LastExpReturn = 4

Line 14: end
  → GetLastFunctionCall() → 0 (함수 호출 없음)
  → printf("Output=%d", 4)
  → 프로그램 종료
```

**최종 스택 상태 변화:**

```
[시작]
STACK: empty

[main 변수 선언 후]
STACK: c=4 → b=2 → a=1 → main함수정의 → f함수정의

[f 호출 후]
STACK: c=2 → b=6 → a=4(매개변수) → f함수정의 → Call(line:13) → c=4 → b=2 → a=1 → ...

[f 종료 후]
STACK: c=4 → b=2 → a=1 → main함수정의 → f함수정의

[프로그램 종료]
Output=4
```

---

## 5.15. 주요 함수 호출 흐름도

```
main()
  │
  ├─→ fgets() ─────────────────────┐
  │                                 │
  │   [라인 종류 판별]              │
  │   ├─ "begin"                    │
  │   │   └─→ Push(type:4)          │
  │   │                             │
  │   ├─ "end"                      │
  │   │   ├─→ Push(type:5)          │
  │   │   ├─→ GetLastFunctionCall() │
  │   │   └─→ 복귀 처리             │
  │   │                             │
  │   ├─ "int ..."                  │
  │   │   ├─→ strtok()              │
  │   │   ├─→ atoi()                │
  │   │   └─→ Push(type:1)          │
  │   │                             │
  │   ├─ "function ..."             │
  │   │   ├─→ strtok()              │
  │   │   ├─→ Push(type:2)          │
  │   │   └─→ [main 체크]           │
  │   │                             │
  │   └─ "((expression))"           │
  │       ├─→ [중위→후위 변환]      │
  │       │   ├─→ isdigit()         │
  │       │   ├─→ PushOp()          │
  │       │   ├─→ PopOp()           │
  │       │   ├─→ Priotry()         │
  │       │   └─→ GetVal()          │
  │       │       └─→ [함수 호출 감지]
  │       │           ├─→ Push(type:3)
  │       │           ├─→ fclose()  │
  │       │           ├─→ fopen()   │
  │       │           └─→ [goto 함수]
  │       │                         │
  │       └─→ [후위 계산]            │
  │           ├─→ PushPostfix()     │
  │           ├─→ PopPostfix()      │
  │           └─→ LastExpReturn 저장│
  │                                 │
  └─────────────────────────────────┘
  
  종료:
  ├─→ fclose()
  ├─→ FreeAll()
  └─→ getch()
```

---

## 5.16. 핵심 기능 정리

### 1. 스택의 역할

| Type | 용도 | exp_data | val | line |
|------|------|----------|-----|------|
| 1 | 변수 | 변수명 | 값 | 0 |
| 2 | 함수 정의 | 함수명 | 0 | 정의 위치 |
| 3 | 함수 호출 | ' ' | -999 | 호출 위치 |
| 4 | begin | ' ' | -999 | -999 |
| 5 | end | ' ' | -999 | -999 |

### 2. 중요 전역 변수

```c
int LastExpReturn       // 마지막 계산된 수식의 결과
int LastFunctionReturn  // 함수 반환 값 (-999 = 아직 반환 안됨)
int CalingFunctionArgVal // 함수에 전달할 인자 값
int foundMain           // main 함수 발견 여부
int WillBreak           // 함수 호출로 인한 루프 중단 플래그
```

### 3. 함수 호출 메커니즘

```
1. 수식에서 함수 호출 감지 (예: f(c))
2. type 3 노드를 스택에 push (복귀 주소 저장)
3. 인자 값을 CalingFunctionArgVal에 저장
4. 파일을 닫고 다시 열어 함수 정의로 이동
5. 함수 실행 (매개변수를 변수로 push)
6. end를 만나면 LastFunctionReturn에 결과 저장
7. 파일을 원래 위치로 되돌림
8. 스택에서 함수의 모든 노드를 pop
9. 수식 파싱 재개 (함수 반환 값 사용)
```

### 4. 변수 스코프 구현

```
GetVal()이 스택을 top부터 검색
→ 가장 최근에 선언된 변수를 먼저 찾음
→ 지역 변수가 같은 이름의 전역 변수를 가림 (shadowing)

예:
STACK: [f의 a=4] [main의 a=1]
GetVal('a') → 4 반환 (f의 지역 변수)

f 함수 종료 후:
STACK: [main의 a=1]
GetVal('a') → 1 반환 (main의 변수)
```

---
