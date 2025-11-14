// 1: File header comment attributing the Basic Interpreter implementation and author email.
// 2: GPL license notice comment.                                                                                                               
// 3: Blank spacer line.                                                                                                                        
// 4: Includes <stdio.h> for standard I/O routines.                                                                                             
// 5: Includes <conio.h> to access getch for console input.
// 6: Includes <string.h> for string utilities such as strcpy and strtok.                                                                       
// 7: Includes <stdlib.h> for memory allocation and conversions like atoi.                                                                      
// 8: Includes <ctype.h> for character classification helpers.                                                                                  
// 9: Blank line separating includes from macros.                                                                                               
// 10: Starts Windows-specific conditional compilation block.                                                                                   
// 11: Defines CLEAR() macro to run cls on Windows consoles.                                                                                    
// 12: Opens alternate branch for non-Windows builds.                                                                                           
// 13: Defines CLEAR() to invoke clear on POSIX-like systems.                                                                                   
// 14: Ends the #ifdef _WIN32 block.                                                                                                            
// 15: Blank separator before type definitions.                                                                                                 
// 16: Declares struct node to represent entries stored in the main stack.                                                                      
// 17: type indicates whether the node is a variable, function, call marker, begin, or end.
// 18: exp_data holds the symbol/identifier character for the node.                                                                             
// 19: val stores integer values associated with variables or evaluation results.                                                               
// 20: line records source line numbers for functions/calls.                                                                                    
// 21: next pointer links to the next node in the stack.                                                                                        
// 22: Closes the struct node definition.                                                                                                       
// 23: Typedefs Node for convenience.                                                                                                           
// 24: Blank spacer.                                                                                                                            
// 25: Declares struct stack containing only a top pointer.                                                                                     
// 26: Typedefs Stack.                                                                                                                          
// 27: Blank spacer.                                                                                                                            
// 28: Defines operator stack node with an operator char and link.                                                                              
// 29: Typedefs opNode.                                                                                                                         
// 30: Blank spacer.                                                                                                                            
// 31: Defines struct opstack containing top pointer to operator nodes.                                                                         
// 32: Typedefs OpStack.                                                                                                                        
// 33: Blank spacer.                                                                                                                            
// 34: Defines postfix evaluation stack node with integer value and next pointer.                                                               
// 35: Typedefs Postfixnode.                                                                                                                    
// 36: Blank spacer.                                                                                                                            
// 37: Defines struct postfixstack holding pointer to the top postfix node.                                                                     
// 38: Typedefs PostfixStack.                                                                                                                   
// 39: Blank spacer before forward declarations.                                                                                                
// 40: Declares helper GetVal returning symbol values.                                                                                          
// 41: Declares GetLastFunctionCall to locate nearest call marker.                                                                              
// 42: Declares FreeAll to clean stack nodes.                                                                                                   
// 43: Declares custom case-insensitive compare my_stricmp.                                                                                     
// 44: Declares rstrip to trim line endings/whitespace.                                                                                         
// 45: Blank line preceding functions.                                                                                                          
// 46: Starts definition of Push to put a Node on the stack.                                                                                    
// 47: Opens function block.                                                                                                                    
// 48: Allocates memory for a new Node.                                                                                                         
// 49: Reports allocation failure and aborts push by returning NULL.                                                                            
// 50: Copies type from source node to heap node.                                                                                               
// 51: Copies val.                                                                                                                              
// 52: Copies exp_data.                                                                                                                         
// 53: Copies line.                                                                                                                             
// 54: Links new node to current top.                                                                                                           
// 55: Updates stack top to the new node.                                                                                                       
// 56: Returns the stack pointer for chaining.                                                                                                  
// 57: Closes function.                                                                                                                         
// 58: Blank spacer.                                                                                                                            
// 59: Begins PushOp, analogous push for operator stack.                                                                                        
// 60: Opens function block.                                                                                                                    
// 61: Allocates a new operator node.                                                                                                           
// 62: Handles allocation failure with message and NULL.                                                                                        
// 63: Sets stored operator character.                                                                                                          
// 64: Links to previous top.                                                                                                                   
// 65: Updates top pointer.                                                                                                                     
// 66: Returns operator stack pointer.                                                                                                          
// 67: Ends PushOp.                                                                                                                             
// 68: Blank line.                                                                                                                              
// 69: Begins PopOp to remove operator stack entries.                                                                                           
// 70: Opens function.                                                                                                                          
// 71: Declares temporary node pointer.                                                                                                         
// 72: Declares variable to hold popped operator.                                                                                               
// 73: Checks if stack empty.                                                                                                                   
// 74: Opens empty-case block.                                                                                                                  
// 75: Returns NUL char when no operator exists.                                                                                                
// 76: Closes if block.                                                                                                                         
// 77: Captures operator from top node.                                                                                                         
// 78: Stores pointer to top node for freeing.                                                                                                  
// 79: Moves top pointer to next node.                                                                                                          
// 80: Frees the popped node.                                                                                                                   
// 81: Returns operator.                                                                                                                        
// 82: Closes PopOp.                                                                                                                            
// 83: Blank line.                                                                                                                              
// 84: Starts PushPostfix for operand stack used during evaluation.                                                                             
// 85: Opens function.                                                                                                                          
// 86: Allocates new postfix node.                                                                                                              
// 87: Handles allocation failure.                                                                                                              
// 88: Stores integer value.                                                                                                                    
// 89: Links to previous top.                                                                                                                   
// 90: Updates top pointer.                                                                                                                     
// 91: Returns stack pointer.                                                                                                                   
// 92: Ends function.                                                                                                                           
// 93: Blank.                                                                                                                                   
// 94: Begins PopPostfix.                                                                                                                       
// 95: Opens function.                                                                                                                          
// 96: Declares temporary pointer.                                                                                                              
// 97: Declares integer for popped value.                                                                                                       
// 98: Checks for empty stack.                                                                                                                  
// 99: Opens empty-case block.                                                                                                                  
// 100: Returns zero when stack empty.                                                                                                          
// 101: Closes if block.                                                                                                                        
// 102: Retrieves top value.                                                                                                                    
// 103: Stores pointer to top node.                                                                                                             
// 104: Advances top pointer.
// 105: Frees popped node.                                                                                                                      
// 106: Returns stored integer.                                                                                                                 
// 107: Ends function.                                                                                                                          
// 108: Blank.                                                                                                                                  
// 109: Starts Pop for main Stack.                                                                                                              
// 110: Opens function.                                                                                                                         
// 111: Declares temp pointer.                                                                                                                  
// 112: Immediately returns if stack empty.                                                                                                     
// 113: Copies exp_data from top node into provided struct.                                                                                     
// 114: Copies type.                                                                                                                            
// 115: Copies line.                                                                                                                            
// 116: Copies val.                                                                                                                             
// 117: Stores pointer to top node.                                                                                                             
// 118: Moves top pointer down the stack.                                                                                                       
// 119: Frees popped node memory.                                                                                                               
// 120: Ends function.                                                                                                                          
// 121: Blank line before helper definitions.                                                                                                   
// 122: Declares isStackEmpty.                                                                                                                  
// 123: Opens function returning boolean indicator.                                                                                             
// 124: Returns whether operator stack top pointer is null.                                                                                     
// 125: Ends isStackEmpty.                                                                                                                      
// 126: Blank.                                                                                                                                  
// 127: Begins Priotry, the misspelled precedence helper.                                                                                       
// 128: Opens function.                                                                                                                         
// 129: Assigns precedence level 1 to + or -.                                                                                                   
// 130: Assigns precedence level 2 to / or *.                                                                                                   
// 131: Returns 0 for other operators.                                                                                                          
// 132: Ends function.                                                                                                                          
// 133: Blank line.                                                                                                                             
// 134: Begins main.                                                                                                                            
// 135: Opens main function block.                                                                                                              
// 136: Declares line buffer for reading source lines.                                                                                          
// 137: Declares dummy buffer used for skipping.                                                                                                
// 138: Declares backup buffer lineyedek.                                                                                                       
// 139: Declares postfix buffer used for expression conversion.                                                                                 
// 140: Declares pointer firstword pointing to tokens.                                                                                          
// 141: Blank line separation.                                                                                                                  
// 142: Declares integer val1, used during postfix eval.                                                                                        
// 143: Declares val2.                                                                                                                          
// 144: Blank line.                                                                                                                             
// 145: Initializes LastExpReturn storing last evaluated expression result.                                                                     
// 146: Initializes LastFunctionReturn sentinel to detect return values.                                                                        
// 147: Initializes CalingFunctionArgVal to hold argument passed into functions.
// 148: Blank line.                                                                                                                             
// 149: Declares reusable tempNode struct for stack operations.                                                                                 
// 150: Blank line.
// 151: Allocates MathStack, the operator stack.                                                                                                
// 152: Declares file pointer filePtr.                                                                                                          
// 153: Allocates postfix evaluation stack CalcStack.                                                                                           
// 154: Declares resultVal accumulator.                                                                                                         
// 155: Allocates main interpreter stack STACK.                                                                                                 
// 156: Blank line.                                                                                                                             
// 157: Initializes curLine to track file line numbers.                                                                                         
// 158: Initializes foundMain flag to detect when main function encountered.                                                                    
// 159: Initializes WillBreak flag controlling expression evaluation.                                                                           
// 160: Blank line.                                                                                                                             
// 161: Guards against allocation failure of stacks.                                                                                            
// 162: Prints error when allocation fails.                                                                                                     
// 163: Returns 1 to signal failure.                                                                                                            
// 164: Closes allocation failure branch.                                                                                                       
// 165: Initializes operator stack top to NULL.                                                                                                 
// 166: Initializes postfix stack top to NULL.                                                                                                  
// 167: Initializes interpreter stack top to NULL.                                                                                              
// 168: Blank line.                                                                                                                             
// 169: Clears the console via macro.                                                                                                           
// 170: Blank line.
// 171: Validates argument count expecting program + filename.                                                                                  
// 172: Opens argument check block.                                                                                                             
// 173: Prints "Incorrect arguments!" if args mismatch.                                                                                         
// 174: Prints proper usage string with executable name.                                                                                        
// 175: Returns 1 to abort when args invalid.                                                                                                   
// 176: Closes argument check block.                                                                                                            
// 177: Blank line.                                                                                                                             
// 178: Attempts to open source file from argument.                                                                                             
// 179: Checks whether fopen succeeded.                                                                                                         
// 180: Opens failure branch.                                                                                                                   
// 181: Prints inability to open file message.                                                                                                  
// 182: Returns 2 indicating IO error.                                                                                                          
// 183: Closes branch.                                                                                                                          
// 184: Blank line.                                                                                                                             
// 185: Starts loop reading each line using fgets.                                                                                              
// 186: Opens loop block.                                                                                                                       
// 187: Declares index k for whitespace normalization.                                                                                          
// 188: Blank.                                                                                                                                  
// 189: Begins iteration to scan characters until null terminator.                                                                              
// 190: Opens inner loop block.                                                                                                                 
// 191: Converts tabs to spaces for consistent tokenizing.                                                                                      
// 192: Increments character index.                                                                                                             
// 193: Closes inner loop.                                                                                                                      
// 194: Blank.                                                                                                                                  
// 195: Trims trailing whitespace/newline via rstrip.                                                                                           
// 196: Copies sanitized line into backup buffer lineyedek.                                                                                     
// 197: Blank.                                                                                                                                  
// 198: Increments current line counter.                                                                                                        
// 199: Resets tempNode.val sentinel.
// 200: Clears tempNode.exp_data.
// 201: Resets stored line number.                                                                                                              
// 202: Resets node type field.                                                                                                                 
// 203: Blank.
// 204: Checks if trimmed line equals "begin" disregarding case.
// 205: Opens condition block.                                                                                                                  
// 206: Ensures interpreter only reacts once main found.                                                                                        
// 207: Opens nested block when inside main.                                                                                                    
// 208: Sets node type 4 representing begin marker.                                                                                             
// 209: Pushes begin marker onto stack.                                                                                                         
// 210: Closes nested block.                                                                                                                    
// 211: Ends "begin" branch.                                                                                                                    
// 212: Starts "end" keyword branch.                                                                                                            
// 213: Opens block.                                                                                                                            
// 214: Confirms code processing only after main discovered.                                                                                    
// 215: Opens nested block.                                                                                                                     
// 216: Declares sline to hold source line of last call.                                                                                        
// 217: Marks node type 5 (end).                                                                                                                
// 218: Pushes end marker.
// 219: Blank line in block for readability.                                                                                                    
// 220: Finds last function call line from stack.                                                                                               
// 221: Checks if no pending call exists.                                                                                                       
// 222: Opens block for top-level output.                                                                                                       
// 223: Prints final expression result as interpreter output.                                                                                   
// 224: Closes branch.                                                                                                                          
// 225: Begins else branch for returning from function.                                                                                         
// 226: Opens block.                                                                                                                            
// 227: Declares j for file rewinding iteration.                                                                                                
// 228: Declares foundCall flag to locate call marker.                                                                                          
// 229: Saves LastExpReturn into LastFunctionReturn.                                                                                            
// 230: Blank.                                                                                                                                  
// 231: Closes current file pointer to rewind.                                                                                                  
// 232: Reopens file from start.                                                                                                                
// 233: Resets current line counter.                                                                                                            
// 234: Loops from start until reaching saved call line.                                                                                        
// 235: Opens loop block.                                                                                                                       
// 236: Reads and discards lines to skip ahead.                                                                                                 
// 237: Increments line counter while skipping.                                                                                                 
// 238: Closes skip loop.
// 239: Blank.
// 240: Enters loop to pop from stack until corresponding call found.                                                                           
// 241: Opens while loop block.
// 242: Pops entries from stack into temp node.                                                                                                 
// 243: Marks foundCall when popped node type equals 3 (function call).                                                                         
// 244: Closes while loop.                                                                                                                      
// 245: Ends else branch.                                                                                                                       
// 246: Closes nested "foundMain" block.                                                                                                        
// 247: Closes "end" handling block.                                                                                                            
// 248: Begins general else processing for other lines.                                                                                         
// 249: Opens block.                                                                                                                            
// 250: Tokenizes line by first space-delimited word.                                                                                           
// 251: Skips processing if line empty after trimming.                                                                                          
// 252: Blank.                                                                                                                                  
// 253: Checks for "int" declaration.                                                                                                           
// 254: Opens branch.                                                                                                                           
// 255: Ensures variables only processed after main seen.                                                                                       
// 256: Opens nested block.                                                                                                                     
// 257: Sets node type 1 (variable).                                                                                                            
// 258: Grabs variable name token.                                                                                                              
// 259: Skips if missing.                                                                                                                       
// 260: Stores first character as identifier.                                                                                                   
// 261: Blank.                                                                                                                                  
// 262: Retrieves next token (should be '=' or literal).                                                                                        
// 263: Skips if missing.                                                                                                                       
// 264: Blank.                                                                                                                                  
// 265: If token equals "=", move to following token.
// 266: Opens branch.                                                                                                                           
// 267: Pulls actual value token.                                                                                                               
// 268: Skips if missing.                                                                                                                       
// 269: Closes "=" branch.                                                                                                                      
// 270: Blank.                                                                                                                                  
// 271: Converts token to integer via atoi.                                                                                                     
// 272: Sets stored source line to 0 for variables.                                                                                             
// 273: Pushes variable node onto stack.                                                                                                        
// 274: Closes nested block.                                                                                                                    
// 275: Ends "int" handling.                                                                                                                    
// 276: Else-if for "function" keyword.                                                                                                         
// 277: Opens branch.                                                                                                                           
// 278: Moves to function name token.                                                                                                           
// 279: Skips if missing.                                                                                                                       
// 280: Blank.                                                                                                                                  
// 281: Marks node type 2 (function declaration).                                                                                               
// 282: Stores first letter of function identifier.                                                                                             
// 283: Records current line where function defined.                                                                                            
// 284: Sets value to zero (unused).                                                                                                            
// 285: Pushes function definition.                                                                                                             
// 286: Blank.                                                                                                                                  
// 287: Checks if function name is "main".                                                                                                      
// 288: Opens branch.                                                                                                                           
// 289: Sets foundMain flag so interpreter begins evaluation.                                                                                   
// 290: Closes branch.                                                                                                                          
// 291: Else branch for non-main functions.                                                                                                     
// 292: Opens branch.                                                                                                                           
// 293: Ensures function parameters only handled once inside main execution.                                                                    
// 294: Opens nested block.                                                                                                                     
// 295: Tokenizes parameter name.                                                                                                               
// 296: Skips if missing.                                                                                                                       
// 297: Reuses tempNode but marks as type 1 variable to represent parameter.                                                                    
// 298: Stores parameter identifier letter.                                                                                                     
// 299: Sets value to previously stored caller argument.                                                                                        
// 300: Sets line to zero (variables).                                                                                                          
// 301: Pushes parameter variable onto stack.                                                                                                   
// 302: Closes nested block.
// 303: Closes else branch.                                                                                                                     
// 304: Ends function handling branch.                                                                                                          
// 305: Else-if identifies expression lines starting with (.                                                                                    
// 306: Opens branch.                                                                                                                           
// 307: Only evaluates expressions after main discovered.                                                                                       
// 308: Opens nested block.                                                                                                                     
// 309: Initializes index i for scanning expression.                                                                                            
// 310: Initializes postfix index y.                                                                                                            
// 311: Blank.                                                                                                                                  
// 312: Resets operator stack top to start new expression.                                                                                      
// 313: Blank.                                                                                                                                  
// 314: Starts loop to parse expression characters.                                                                                             
// 315: Opens loop block.                                                                                                                       
// 316: Checks for digits representing literal integers.                                                                                        
// 317: Opens branch.
// 318: Copies digit directly into postfix buffer.                                                                                              
// 319: Increments postfix index.                                                                                                               
// 320: Closes digit branch.                                                                                                                    
// 321: Else-if for closing parenthesis.                                                                                                        
// 322: Opens branch.                                                                                                                           
// 323: Ensures operator stack not empty before popping.                                                                                        
// 324: Opens nested block.                                                                                                                     
// 325: Pops operator and writes to postfix buffer.                                                                                             
// 326: Increments postfix index.                                                                                                               
// 327: Closes nested block.                                                                                                                    
// 328: Closes parentheses branch.                                                                                                              
// 329: Else-if handles arithmetic operators.                                                                                                   
// 330: Opens branch.                                                                                                                           
// 331: Checks if operator stack empty.                                                                                                         
// 332: Opens nested block.                                                                                                                     
// 333: Pushes operator when stack empty.                                                                                                       
// 334: Closes nested block.                                                                                                                    
// 335: Else handles precedence/resolution when stack not empty.                                                                                
// 336: Opens nested block.                                                                                                                     
// 337: Compares precedence with operator at stack top.                                                                                         
// 338: Opens nested-if block for lower/equal precedence.                                                                                       
// 339: Moves top operator to postfix when precedence dictates.                                                                                 
// 340: Increments postfix index.                                                                                                               
// 341: Pushes current operator afterwards.                                                                                                     
// 342: Closes nested-if.
// 343: Else branch for higher precedence (just push).                                                                                          
// 344: Opens branch.                                                                                                                           
// 345: Pushes operator without popping.                                                                                                        
// 346: Closes branch.                                                                                                                          
// 347: Closes operator-handling block.                                                                                                         
// 348: Closes else-if chain.                                                                                                                   
// 349: Else-if for alphabetic characters representing identifiers.                                                                             
// 350: Opens block.                                                                                                                            
// 351: Declares codeline to receive function definition line.                                                                                  
// 352: Declares dummyint placeholder for GetVal.                                                                                               
// 353: Retrieves stored value or function metadata via GetVal.                                                                                 
// 354: Blank.                                                                                                                                  
// 355: If return is valid variable value (not -1, -999).                                                                                       
// 356: Opens branch.                                                                                                                           
// 357: Encode integer (0-9) to ASCII digit and push into postfix.
// 358: Increment postfix index.                                                                                                                
// 359: Closes branch.                                                                                                                          
// 360: Else handles function calls or unresolved identifiers.                                                                                  
// 361: Opens else block.                                                                                                                       
// 362: Checks whether we already have a pending function return value.                                                                         
// 363: Opens branch when no return yet.                                                                                                        
// 364: Declares loop variable j.                                                                                                               
// 365: Marks tempNode as type 3 (function call).                                                                                               
// 366: Stores current line to allow returning after call.                                                                                      
// 367: Pushes call marker onto stack.                                                                                                          
// 368: Blank.                                                                                                                                  
// 369: Looks up argument value from expression (assumes single-digit).                                                                         
// 370: Blank.                                                                                                                                  
// 371: Closes file to restart reading from target function line.                                                                               
// 372: Reopens file again.                                                                                                                     
// 373: Resets current line counter.                                                                                                            
// 374: Blank.                                                                                                                                  
// 375: Loops to advance file pointer to callee definition line.                                                                                
// 376: Reads dummy lines while skipping.                                                                                                       
// 377: Increments line counter.                                                                                                                
// 378: Continues skipping until reaching function.                                                                                             
// 379: Line counter update.                                                                                                                    
// 380: Blank.                                                                                                                                  
// 381: Sets WillBreak so evaluation stops and interpreter jumps to callee.                                                                     
// 382: Breaks out of parsing loop to restart reading file.                                                                                     
// 383: Closes branch for performing call.                                                                                                      
// 384: Else branch executed when LastFunctionReturn already available.                                                                         
// 385: Opens block.                                                                                                                            
// 386: Inserts return value into postfix expression.                                                                                           
// 387: Increments postfix pointer.                                                                                                             
// 388: Skips ahead i by 3 to move past identifier and parentheses (assumes format).                                                            
// 389: Resets LastFunctionReturn sentinel.                                                                                                     
// 390: Closes block.                                                                                                                           
// 391: Closes else-if for function/call resolution.                                                                                            
// 392: Closes main parsing while block.                                                                                                        
// 393: Increments scanning index i.                                                                                                            
// 394: Closes while loop scanning characters.                                                                                                  
// 395: Blank.                                                                                                                                  
// 396: Checks if evaluation should proceed (no call break).                                                                                    
// 397: Opens block.                                                                                                                            
// 398: Empties remaining operators by popping stack.                                                                                           
// 399: Opens loop.                                                                                                                             
// 400: Appends popped operator to postfix buffer.                                                                                              
// 401: Increments postfix pointer.                                                                                                             
// 402: Closes loop iteration.                                                                                                                  
// 403: Blank line.                                                                                                                             
// 404: Null-terminates postfix string.                                                                                                         
// 405: Blank.                                                                                                                                  
// 406: Resets i before evaluating postfix.                                                                                                     
// 407: Clears postfix evaluation stack top.                                                                                                    
// 408: Starts evaluation loop over postfix characters.                                                                                         
// 409: Opens loop block.                                                                                                                       
// 410: Checks if current char is digit operand.                                                                                                
// 411: Opens branch.                                                                                                                           
// 412: Pushes numeric value (digit minus '0') on postfix stack.                                                                                
// 413: Closes branch.                                                                                                                          
// 414: Else-if for arithmetic operators.                                                                                                       
// 415: Opens branch.                                                                                                                           
// 416: Pops first operand.                                                                                                                     
// 417: Pops second operand.                                                                                                                    
// 418: Blank.                                                                                                                                  
// 419: Switch executes operator-specific arithmetic.                                                                                           
// 420: Case '+' adds operands.                                                                                                                 
// 421: Case '-' subtracts.                                                                                                                     
// 422: Case '/' divides (trunc integer).                                                                                                       
// 423: Case '*' multiplies.                                                                                                                    
// 424: Closes switch.
// 425: Pushes computed result back on evaluation stack.                                                                                        
// 426: Closes operator branch.                                                                                                                 
// 427: Increments index i.                                                                                                                     
// 428: Ends while loop iteration.                                                                                                              
// 429: Closes evaluation loop.                                                                                                                 
// 430: Blank.                                                                                                                                  
// 431: Stores final postfix stack top value as LastExpReturn.                                                                                  
// 432: Closes block for evaluation when not breaking.                                                                                          
// 433: Resets WillBreak flag before continuing file reading.                                                                                   
// 434: Closes nested expression handling block.                                                                                                
// 435: Closes outer if (foundMain) block for expression lines.                                                                                 
// 436: Closes else if (firstword[0] == '(') branch.                                                                                            
// 437: Closes general else for tokenized statements.                                                                                           
// 438: Closes line-reading while loop.                                                                                                         
// 439: Blank.                                                                                                                                  
// 440: Closes file when finished reading.
// 441: Frees stack memory via FreeAll.                                                                                                         
// 442: Blank line.                                                                                                                             
// 443: Prompts user to press a key before exiting.                                                                                             
// 444: Waits for keypress via getch.                                                                                                           
// 445: Returns 0 from main for successful termination.                                                                                         
// 446: Closes main.                                                                                                                            
// 447: Blank line preceding helper definitions.                                                                                                
// 448: Begins FreeAll.                                                                                                                         
// 449: Initializes iterator head to stack top.                                                                                                 
// 450: Loops through nodes until null.                                                                                                         
// 451: Inside loop, holds current node pointer.                                                                                                
// 452: Advances head to next node.                                                                                                             
// 453: Frees previously saved node.                                                                                                            
// 454: Closes loop.                                                                                                                            
// 455: Clears stack top pointer.                                                                                                               
// 456: Returns NULL to caller (though unused).                                                                                                 
// 457: Ends FreeAll.                                                                                                                           
// 458: Blank.                                                                                                                                  
// 459: Begins GetLastFunctionCall.                                                                                                             
// 460: Sets iterator to top of stack.                                                                                                          
// 461: Loops through stack nodes.                                                                                                              
// 462: Inside loop block.
// 463: Returns first encountered line number for type 3 nodes.                                                                                 
// 464: Moves to next node otherwise.                                                                                                           
// 465: Closes loop.                                                                                                                            
// 466: Returns 0 if no call markers present.                                                                                                   
// 467: Ends helper.                                                                                                                            
// 468: Blank.                                                                                                                                  
// 469: Begins GetVal.                                                                                                                          
// 470: Declares iterator head.                                                                                                                 
// 471: Resets line output parameter to 0.                                                                                                      
// 472: Returns sentinel -999 if stack empty.                                                                                                   
// 473: Initializes iterator from top.                                                                                                          
// 474: Loops through nodes.                                                                                                                    
// 475: Opens loop block.                                                                                                                       
// 476: Matches requested identifier against stored exp_data.                                                                                   
// 477: Opens branch.                                                                                                                           
// 478: Returns variable value immediately for type 1 nodes.                                                                                    
// 479: Else-if handles type 2 functions: records line number and returns -1 as sentinel.                                                       
// 480: Closes branch.                                                                                                                          
// 481: Advances to next node if no match.                                                                                                      
// 482: Continues loop.                                                                                                                         
// 483: Returns -999 when identifier unresolved.                                                                                                
// 484: Ends GetVal.                                                                                                                            
// 485: Blank line.                                                                                                                             
// 488: Loops until both strings exhausted.
// 489: Converts character from a to lowercase.
// 490: Converts character from b.
// 491: If characters differ, returns their difference.
// 496: Ends function.
// 497: Blank.
// 498: Begins rstrip.
// 499: Determines string length.
// 500: Loops while trailing characters are newline, carriage return, or space.
// 501: Inside loop, replaces trailing whitespace with null terminator by decrementing length.
// 502: Continues trimming until no trailing whitespace remains.
// 503: Ends function and file.