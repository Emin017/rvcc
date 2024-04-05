#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TK_PUNCT,
  TK_NUM,
  TK_EOF
} TokenKind;

typedef struct Token Token ;
struct Token {
	TokenKind Kind;
	Token *Next;
	int Val;
	char *Loc; // Location of the token
	int Len;
};

static void error(char *Fmt, ...) {
// va_list为指向参数的指针
	va_list VA;
// va_start初始化VA指针，第一个参数为VA指针，第二个参数为Fmt
	va_start(VA, Fmt);
// vfprintf可以输出va_list类型的参数
	vfprintf(stderr, Fmt, VA);
// 在结尾加上换行符
	fprintf(stderr, "\n");
// va_end结束VA指针
	va_end(VA);
// 退出程序
	exit(1);
}

static bool equal(Token *Tok, char *Str) {
	// 比较字符串LHS(左部分), RHS(右部分)的前N位，S2长度要大于等于N
	return memcmp(Tok->Loc, Str, Tok->Len) == 0 && Str[Tok->Len] == '\0';
}

static int getNumber(Token *Tok) {
	if (Tok->Kind != TK_NUM)
		error("expected a number");
	return Tok->Val;
}

static Token *newToken(TokenKind Kind, char *Loc, char *End) {
	// 新建一个链表节点
	Token *Tok = calloc(1, sizeof(Token));
	Tok->Kind = Kind;
	Tok->Loc = Loc;
	Tok->Len = End - Loc;
	return Tok;
}

static Token *skip(Token *Tok, char *Str) {
	if (!equal(Tok, Str))
		error("expected '%s'", Str);
	return Tok->Next;
}

static Token *tokenize(char *P) {
	// 新建一个链表
	// Head为头指针，Cur为当前指针
	// Head.Next为第一个元素
	Token Head = {};
	Token *Cur = &Head;

	while (*P) {
		if (isspace(*P)) {
			// 跳过空格
			++P;
			continue;
		}

		if (isdigit(*P)) {
			// 识别数字，将数字转换为长整型
			// 将数字存入链表中
			// 不使用Head存放节点，因此每次都放在Cur->Next中
			Cur->Next = newToken(TK_NUM, P, P);
			// 前进一个节点
			Cur = Cur->Next;
			const char *OldPtr = P;
			Cur->Val = strtoul(P, &P, 10);
			Cur->Len = P - OldPtr;
			continue;
		}

		if (*P == '+' || *P == '-') {
			// 识别出加减符号，存放入链表中
      // !不使用Head存放节点，因此每次都放在Cur->Next中
			Cur->Next = newToken(TK_PUNCT, P, P + 1);
      // 前进一个节点
      Cur = Cur->Next;
			++P;
			continue;
		}


		error("invalid token: %c", *P);
	}
	// 识别结束，添加EOF结束链表
  // !不使用Head存放节点，因此每次都放在Cur->Next中
	Cur->Next = newToken(TK_EOF, P, P);
	return Head.Next;
}

int main(int Argc, char **Argv) {
  // 判断传入程序的参数是否为2个，Argv[0]为程序名称，Argv[1]为传入的第一个参数
  if (Argc != 2) {
    // 异常处理，提示参数数量不对。
    // fprintf，格式化文件输出，往文件内写入字符串
    // stderr，异常文件（Linux一切皆文件），用于往屏幕显示异常信息
    // %s，字符串
    error(stderr, "%s: invalid number of arguments\n", Argv[0]);
    // 程序返回值不为0时，表示存在错误
    return 1;
  }
  char *P = Argv[1];

	Token *Tok = tokenize(Argv[1]);
  // 声明一个全局main段，同时也是程序入口段
  printf("  .globl main\n");
  // main段标签
  printf("main:\n");
  // li为addi别名指令，加载一个立即数到寄存器中
  // 传入程序的参数为str类型，因为需要转换为需要int类型，
  // atoi为“ASCII to integer”
  // 传入&P, 即char**，为了修改P的值
  // 表达式分解为num (op num)(op num)...
  printf("  li a0, %d\n", getNumber(Tok));
	Tok = Tok->Next;
  // 解析（op num）
	while (Tok->Kind != TK_EOF) {
		if (equal(Tok, "+")) {
			Tok = Tok->Next;
			printf("  addi a0, a0, %d\n", getNumber(Tok));
			Tok = Tok->Next;
			continue;
		}
		// RISCV没有subi指令，因此需要转换为addi指令
    Tok = skip(Tok, "-");
    printf("  addi a0, a0, -%d\n", getNumber(Tok));
    Tok = Tok->Next;
	}
  // ret为jalr x0, x1, 0别名指令，用于返回子程序
  // 输出最后运算结果
  printf("  ret\n");

  return 0;
}
