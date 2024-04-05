#include <stdio.h>
#include <stdlib.h>

int main(int Argc, char **Argv) {
  // 判断传入程序的参数是否为2个，Argv[0]为程序名称，Argv[1]为传入的第一个参数
  if (Argc != 2) {
    // 异常处理，提示参数数量不对。
    // fprintf，格式化文件输出，往文件内写入字符串
    // stderr，异常文件（Linux一切皆文件），用于往屏幕显示异常信息
    // %s，字符串
    fprintf(stderr, "%s: invalid number of arguments\n", Argv[0]);
    // 程序返回值不为0时，表示存在错误
    return 1;
  }
  char *P = Argv[1];

  // 声明一个全局main段，同时也是程序入口段
  printf("  .globl main\n");
  // main段标签
  printf("main:\n");
  // li为addi别名指令，加载一个立即数到寄存器中
  // 传入程序的参数为str类型，因为需要转换为需要int类型，
  // atoi为“ASCII to integer”
  // 传入&P, 即char**，为了修改P的值
  // 表达式分解为num (op num)(op num)...
  printf("  li a0, %ld\n", strtol(P, &P, 10));
  // 解析（op num）
  while(*P) {
	// 解析‘+’
    if (*P == '+') {
	  // P++，指针后移
	  ++P;
	  // strtol为“string to long”，将字符串转换为长整型,传入&P, 即char**，为了修改P的值, 传入10，表示十进制
      printf("  addi a0, a0, %ld\n", strtol(P, &P, 10));
      continue;
	}

	// 解析‘-’
    if (*P == '-') {
	  ++P;
      printf("  addi a0, a0, -%ld\n", strtol(P, &P, 10));
      continue;
	}
	fprintf(stderr, "unexpected character: '%c'\n", *P);
	return 1;
  }
  // ret为jalr x0, x1, 0别名指令，用于返回子程序
  // 输出最后运算结果
  printf("  ret\n");

  return 0;
}
