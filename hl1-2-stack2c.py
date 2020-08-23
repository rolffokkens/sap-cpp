import sys
import re
from enum import Enum

I_SETFP   = 0x00
I_SETSP   = 0x10
I_LDTRC   = 0x20
I_LDTRL   = 0x30
I_FETCH   = 0x40
I_STORE   = 0x50
I_PUSH    = 0x60
I_POP     = 0x70
I_CALL    = 0x80
I_RET     = 0x90
I_ADD     = 0xA2
I_SUB     = 0xA3
I_CMP     = 0xB0
I_SWAP    = 0xC0
I_OUT     = 0xD0
I_JUMP    = 0xE0
I_JUMP_Z  = 0xE4
I_JUMP_NZ = 0xE5
I_JUMP_C  = 0xE6
I_JUMP_NC = 0xE7
I_HALT    = 0xF0

class Token(Enum):
    EOF     =  0
    NUMBER  =  1
    STRING  =  2
    IDENT   =  3
    INT     =  4
    LRBRACK =  5
    RRBRACK =  6
    BEGIN   =  7
    END     =  8
    WHILE   =  9
    IF      = 10
    ELSE    = 11
    SEMICOL = 12
    COMMA   = 13
    LT      = 14
    LE      = 15
    EQ      = 16
    GE      = 17
    GT      = 18
    NE      = 19
    NOT     = 20
    LSBRACK = 21
    RSBRACK = 22
    RETURN  = 23
    ADD     = 24
    SUB     = 25
    ASSIGN  = 26
    OUT     = 26

keywords = {
    "int"    : Token.INT    ,
    "for"    : Token.WHILE  ,
    "("      : Token.LRBRACK ,
    ")"      : Token.RRBRACK ,
    "["      : Token.LSBRACK ,
    "]"      : Token.RSBRACK ,
    "{"      : Token.BEGIN ,
    "}"      : Token.END ,
    ";"      : Token.SEMICOL ,
    ","      : Token.COMMA ,
    "+"      : Token.ADD ,
    "-"      : Token.SUB ,
    "="      : Token.ASSIGN ,
    "while"  : Token.WHILE ,
    "if"     : Token.IF ,
    "else"   : Token.ELSE ,
    "return" : Token.RETURN ,
    "out"    : Token.OUT ,

    "<"      : Token.LT ,
    "<="     : Token.LE ,
    "=<"     : Token.LE ,
    "=="     : Token.EQ ,
    ">="     : Token.GE ,
    "=>"     : Token.GE ,
    ">"      : Token.GT ,
    "<>"     : Token.NE ,
    "!="     : Token.NE ,
    "!"      : Token.NOT ,
}

cmp_code = {
    Token.LT : [(I_JUMP_NC, 1)                ],
    Token.LE : [(I_JUMP_C,  0), (I_JUMP_NZ, 1)],
    Token.EQ : [(I_JUMP_NZ, 1)                ],
    Token.GE : [(I_JUMP_C,  1)                ],
    Token.GT : [(I_JUMP_C,  1), (I_JUMP_Z,  1)],
    Token.NE : [(I_JUMP_NZ, 1)                ],
}

class Ident(Enum):
    VAR  = 1
    FUNC = 2

line = ""
linecnt = 0
token = (None, None)
memtop = {0 : 0xfffe}

def next_token ():
    global line
    global linecnt
    global token
    word  = None
    deflt = None

    line = line.lstrip()
    while line == "":
        line = sys.stdin.readline()
        linecnt += 1
        if not line:
            token = (Token.EOF, None)
            return
        line = line.lstrip()
    if line[0] >= "a" and line[0] <= "z" or line[0] >= "A" and line[0] <= "Z":
       (s, e) = re.search ('[^a-zA-Z0-9]', line).span ()
       word  = line[0:s]
       line  = line[s:]
       deflt = Token.IDENT
    elif line[0] >= "0" and line[0] <= "9":
       (s, e) = re.search ('[^0-9]', line).span ()
       word  = line[0:s]
       line  = line[s:]
       token = (Token.INT, int (word))
       return
    elif line[0] == "!" or line[0] == "<" or line[0] == "=" or line[0] == ">":
       (s, e) = re.search ('[^<=>]', line[1:]).span ()
       word  = line[0:s+1]
       line  = line[s+1:]
    else:
       word  = line[0]
       line  = line[1:]

    for k, v in keywords.items ():
       if k == word:
           token = (v, word)
           return

    token = (deflt, word)

def set_16bit_word (addr, word):
    global code
    code[addr  ] = ( word       & 0xff)
    code[addr+1] = ((word >> 8) & 0xff)

def gen_16bit_code (instr, val):
    global code

    code.append (instr            )
    code.append ( val       & 0xff)
    code.append ((val >> 8) & 0xff)

def compile_expr (scope, offset):
    global code
    global codeptr

    op = None

    while True:
        if op:
            code.append (I_PUSH)
        (t, w) = token
        if   t == Token.LRBRACK:
            next_token ()
            compile_expr (scope, offset)
            (t, w) = token
            if t != Token.RRBRACK:
                raise Exception ("Unexpected token \"%s\" at line %d, expected \")\"" % (w, linecnt))
        elif t == Token.INT:
            gen_16bit_code (I_LDTRC, w)
        elif t == Token.IDENT:
            typ = None
            for i in range (scope, -1, -1):
                if w in idents[i]:
                    (typ, atr) = idents[i][w]
                    break
            if typ == None:
                raise Exception ("Reference to unknown identifier \"%s\" at line %d" % (w, linecnt))
            if typ == Ident.VAR:
                instr = I_LDTRC if i == 0 else I_LDTRL
                addr  = atr     if i == 0 else atr + offset
                gen_16bit_code (instr, addr)
                code.append (I_FETCH           )
            else:
                (addr, argsize) = atr
                next_token ()
                (t, w) = token
                if t != Token.LRBRACK:
                    raise Exception ("Unexpected token \"%s\" at line %d, expected \"(\"" % (w, linecnt))
                while True:
                    next_token ()
                    (t, w) = token
                    if t == Token.RRBRACK:
                        break
                    compile_expr (scope, offset)
                    code.append (I_PUSH)
                    (t, w) = token
                    if t != Token.COMMA:
                        break
                if t != Token.RRBRACK:
                    raise Exception ("Unexpected token \"%s\" at line %d, expected \")\"" % (w, linecnt))
                gen_16bit_code (I_LDTRC, addr)
                code.append (I_CALL)
        else:
            raise Exception ("Unexpected token \"%s\" in expression at line %d" % (w, linecnt))
        if op:
            if op in cmp_code:
                code.append (I_CMP)
                gen_16bit_code (I_LDTRC, 0)
                code.append (I_PUSH)
                then_addr = len (code) + 4 * len (cmp_code[op])
                else_addr = then_addr  + 4 + 1
                for (inst, is_else) in cmp_code[op]:
                    gen_16bit_code (I_LDTRC, else_addr if is_else else then_addr)
                    code.append (inst)
                code.append (I_POP)
                gen_16bit_code (I_LDTRC, 1)
                code.append (I_PUSH)
                code.append (I_POP)
            else:
                code.append (I_ADD if op == Token.ADD else I_SUB)

        next_token ()
        (t, w) = token
        if t != Token.ADD and t != Token.SUB and not t in cmp_code:
            break
        op = t 
        next_token ()

def compile_stmt_return (offset):
    global code

    next_token ()
    compile_expr (1, offset)
    code.append (I_RET)

def compile_stmt_out (offset):
    global code

    next_token ()
    compile_expr (1, offset)
    code.append (I_OUT)

def compile_stmt_while (offset):
    global code

    next_token ()
    (t, w) = token
    if t != Token.LRBRACK:
        raise Exception ("Unexpected token \"%s\" at line %d, expected \"(\"" % (w, linecnt))

    loop_addr = len (code)

    next_token ()
    compile_expr (1, offset)
    (t, w) = token
    if t != Token.RRBRACK:
        raise Exception ("Unexpected token \"%s\" at line %d, expected \")\"" % (w, linecnt))
    next_token ()

    code.append (I_PUSH)
    gen_16bit_code (I_LDTRC, 0)
    code.append (I_CMP)
    brk = [ len (code) + 1 ]
    gen_16bit_code (I_LDTRC, 0)
    code.append (I_JUMP_Z)
    compile_stmt (offset)

    gen_16bit_code (I_LDTRC, loop_addr)
    code.append (I_JUMP)

    for addr in brk:
        set_16bit_word (addr, len (code))

def compile_stmt_if (offset):
    global code

    next_token ()
    (t, w) = token
    if t != Token.LRBRACK:
        raise Exception ("Unexpected token \"%s\" at line %d, expected \"(\"" % (w, linecnt))

    next_token ()
    compile_expr (1, offset)
    (t, w) = token
    if t != Token.RRBRACK:
        raise Exception ("Unexpected token \"%s\" at line %d, expected \")\"" % (w, linecnt))
    next_token ()

    code.append (I_PUSH)
    gen_16bit_code (I_LDTRC, 0)
    code.append (I_CMP)
    else_ptr = len (code) + 1
    gen_16bit_code (I_LDTRC, 0)
    code.append (I_JUMP_Z)
    compile_stmt (offset)

    (t, w) = token
    if t == Token.ELSE:
        then_ptr = len (code) + 1
        gen_16bit_code (I_LDTRC, 0)
        code.append (I_JUMP)
        else_addr = len(code)

        next_token ()
        compile_stmt (offset)
    else:
        then_ptr = None
        else_addr = len(code)

    set_16bit_word (else_ptr, else_addr)
    if then_ptr:
        set_16bit_word (then_ptr, len(code))

def compile_stmt (offset):
    (t, w) = token
    if   t == Token.BEGIN:
        next_token ()
        compile_stmt_list (offset)
        (t, w) = token
        if t != Token.END:
            raise Exception ("Unexpected token \"%s\" at line %d, expected \"}\"" % (w, linecnt))
        next_token ()
    elif t == Token.RETURN:
        compile_stmt_return (offset)
    elif t == Token.OUT:
        compile_stmt_out (offset)
    elif t == Token.WHILE:
        compile_stmt_while (offset)
    elif t == Token.IF:
        compile_stmt_if (offset)
    elif t == Token.IDENT:
        typ = None
        for i in range (1, -1, -1):
            if w in idents[i]:
                (typ, atr) = idents[i][w]
                break
        if typ == None:
            raise Exception ("Reference to unknown variable \"%s\" at line %d" % (w, linecnt))
        if typ != Ident.VAR:
            raise Exception ("Reference to unknown variable \"%s\" at line %d" % (w, linecnt))

        instr = I_LDTRC if i == 0 else I_LDTRL
        addr  = atr     if i == 0 else atr + offset

        next_token ()
        (t, w) = token
        if t != Token.ASSIGN:
            raise Exception ("Unexpected token \"%s\" at line %d, expected \"=\"" % (w, linecnt))

        next_token ()
        compile_expr (1, offset)
        code.append (I_PUSH)

        gen_16bit_code (instr, addr)
        code.append (I_STORE)
    return

def compile_stmt_list (offset):
    while True:
        compile_stmt (offset)

        (t, w) = token
        if t != Token.SEMICOL:
            break

        next_token ()
    return

def compile_func_decl (ident):
    global memtop
    global idents
    global code

    memtop[1] = 0
    idents[1] = {}

    addr = len (code)

    next_token ()
    (t, w) = token
    while True:
        compile_decl (1)
        (t, w) = token
        if t != Token.COMMA:
            break
        next_token ()
    if t != Token.RRBRACK:
        raise Exception ("Unexpected token \"%s\" at line %d, expected \")\"" % (w, linecnt))
    next_token ()
    (t, w) = token

    memtop[1] -= 4
    argoffset = -memtop[1]

    idents[0].update ({ ident : (Ident.FUNC, (addr, argoffset)) })

    if t != Token.BEGIN:
        raise Exception ("Unexpected token \"%s\" at line %d, expected \"{\"" % (w, linecnt))
    next_token ()
    (t, w) = token

    compile_decl_list (1)
    fsize = memtop[1] + argoffset
    gen_16bit_code (I_SETSP, fsize)
    compile_stmt_list (argoffset)
    code.append (I_RET)

    (t, w) = token
    if t != Token.END:
        raise Exception ("Unexpected token \"%s\" at line %d, expected \"}\"" % (w, linecnt))
    next_token ()
    (t, w) = token

def compile_decl (scope):
    global memtop
    global idents

    (t, w) = token
    if t != Token.INT:
        return

    next_token ()
    (t, w) = token
    if t != Token.IDENT:
        return
    if w in idents[scope]:
        raise Exception ("Doubly declared identifier \"%s\" at line %d" % (w, linecnt))
    ident = w
    next_token ()
    (t, w) = token

    if t == Token.LRBRACK:
        if scope == 0:
            compile_func_decl (ident)
    else:
        memtop[scope] -= 2
        idents[scope].update ({ ident : (Ident.VAR, memtop[scope]) })

def compile_decl_list (scope):
    while True:
        compile_decl (scope)
        (t, w) = token
        if t != Token.SEMICOL:
            break
        next_token ()
    return

def main():
    global code
    global idents
    global memtop

    idents = {0: {}}
    code   = bytearray(4)

    next_token ()
    compile_decl_list (0)
    (t, w) = token
    if t == None:
        raise Exception ("Bad token \"%s\" at line %d" % (w, linecnt))
    if t != Token.EOF:
        raise Exception ("Unexpected token \"%s\" at line %d" % (w, linecnt))
    jump = len (code)
    if not "main" in idents[0]:
        raise Exception ("Missing main() function")
    (typ, atr) = idents[0]["main"]
    if typ != Ident.FUNC:
        raise Exception ("Missing main() function")
    (addr, stacksiz) = atr
    gen_16bit_code (I_SETFP, memtop[0])
    gen_16bit_code (I_LDTRC, 0xfffe)
    code.append (I_FETCH)
    code.append (I_PUSH)
    gen_16bit_code (I_LDTRC, addr)
    code.append (I_CALL)
    code.append (I_HALT)

    code[0] = I_LDTRC
    set_16bit_word (1, jump)
    code[3] = I_JUMP

    sys.stdout.buffer.write (code)


if __name__ == "__main__":
    main()
