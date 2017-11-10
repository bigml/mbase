### 单词分组

    char *string = "ping";
    err = mre_compile(reo, "ping|PING");
    mre_dump(reo);
    MTEST_ASSERT(mre_match(reo, string, false) == true);
    _dump_res(reo, string);

    0: split 14
    1:     lpar 0
    2:         split 8
    3:             char 'p'
    4:             char 'i'
    5:             char 'n'
    6:             char 'g'
    7:             jump relative 12 2147483647
    8:         char 'P'
    9:         char 'I'
   10:         char 'N'
   11:         char 'G'
   12:     rpar 0
   13:     end
   14: anynl
   15: jump absolute 0
0's 0 match: 0 ping



### Group (no capture, positive lookahead, negative lookahead)

  * ()     group and substring capturing

    err = mre_compile(reo, "(ping)");
    char *string = "you ping me";

    0: split 10
    1:     lpar 0
    2:         lpar 1                 ***************** 结果会被保存
    3:             char 'p'
    4:             char 'i'
    5:             char 'n'
    6:             char 'g'
    7:         rpar 1
    8:     rpar 0
    9:     end
   10: anynl
   11: jump absolute 0
0's 0 match: 4 ping
0's 1 match: 4 ping



  * (?:)   no capture

    err = mre_compile(reo, "(?:ping)");
    char *string = "you ping me";

    0: split 10
    1:     lpar 0                 ***************** 结果不会被保存
    2:         lpar 0
    3:             char 'p'
    4:             char 'i'
    5:             char 'n'
    6:             char 'g'
    7:         rpar 0
    8:     rpar 0
    9:     end
   10: anynl
   11: jump absolute 0
0's 0 match: 4 ping



  * (?=)   positive lookahead

    err = mre_compile(reo, "Win(?=95|98|NT).*in");
    char *string = "build WinNT inside";

    0: split 27
    1:     lpar 0
    2:         char 'W'
    3:         char 'i'
    4:         char 'n'
    5:         pla 17                 *****************
    6:             split 14
    7:                 split 11
    8:                     char '9'
    9:                     char '5'
   10:                     jump relative 13 2147483647
   11:                 char '9'
   12:                 char '8'
   13:                 jump relative 16 2147483647
   14:             char 'N'
   15:             char 'T'
   16:             end                 *****************
   17:         split 22
   18:             any
   19:             split 22
   20:                 any
   21:                 jump relative 19 2147483645
   22:             jump relative 23 0
   23:         char 'i'
   24:         char 'n'
   25:     rpar 0
   26:     end
   27: anynl
   28: jump absolute 0
1's 0 match: 6 WinNT in


  * (?!)   negative lookahead

    err = mre_compile(reo, "Win(?!95|98|NT).*in");
    char *string = "build WinMe inside";


    0: split 27
    1:     lpar 0
    2:         char 'W'
    3:         char 'i'
    4:         char 'n'
    5:         nla 17                 *****************
    6:             split 14
    7:                 split 11
    8:                     char '9'
    9:                     char '5'
   10:                     jump relative 13 2147483647
   11:                 char '9'
   12:                 char '8'
   13:                 jump relative 16 2147483647
   14:             char 'N'
   15:             char 'T'
   16:             end                 *****************
   17:         split 22
   18:             any
   19:             split 22
   20:                 any
   21:                 jump relative 19 2147483645
   22:             jump relative 23 0
   23:         char 'i'
   24:         char 'n'
   25:     rpar 0
   26:     end
   27: anynl
   28: jump absolute 0


参考代码:

            case I_PLA:
                if (!_execute(reo, pc + 1, sp, igcase)) {
                    goto river;
                } else {
                    pc = _pc_relative(reo, pc, pc->b);
                    continue;
                }
            case I_NLA:
                if (_execute(reo, pc + 1, sp, igcase)) {
                    goto river;
                } else {
                    pc = _pc_relative(reo, pc, pc->b);
                    continue;
                }
