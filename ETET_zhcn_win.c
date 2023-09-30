#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <windows.h>

#define KEY_MAX 1000
#define KET_MIN 10
#define ADDRESS_L 500

int *enc(char*, int); //用于加密操作的函数
char *dec(int*, int); //用于解密操作的函数

int randZ(void); //获取质数
int *enc_k(char*, int, unsigned long long*);
char *dec_k(int*, int, unsigned long long*);

int poww(int, unsigned long long, unsigned long long);
void keyg(void);
void encall(void);
void decall(void);

int main (void) //加密文本文件
{
    SetConsoleOutputCP(65001);
    printf("简单英文文本加密器\n");
    printf("请选择模式：\n");
    printf("A. 生成密钥   B. 加密\n");
    printf("C. 解密      q. 退出\n");
    char ch = getchar();
    
    while (ch != 'q' && ch != 'Q')
    {
        while (getchar() != '\n')
            continue;
        switch (ch)
        {
        case 'a':
            ;
        case 'A':
            keyg();
            break;
        
        case 'b':
            ;
        case 'B':
            encall();
            break;
        
        case 'c':
            ;
        case 'C':
            decall();
            break;
        
        default:
            printf("请输入一个有效选项！\n");
            break;
        }
        printf("请选择模式：\n");
        printf("A. 生成密钥   B. 加密\n");
        printf("C. 解密      q. 退出\n");
        ch = getchar();
    }
    return 0;
}

int *enc(char* bf, int a)
{
    int *af = NULL;
    af = (int*)malloc(a * sizeof(int));
    int i = 0;
    while (i < a)
    {
        *(af + i) = (int)(*(bf + i)) * (*(bf + i));
        i++;
    }
    return af;
}

char *dec(int *bf, int a)
{
    char *af = NULL;
    af = (char*)malloc(a * sizeof(char));
    int i = 0;
    while (i < a)
    {
        *(af + i) = (int)sqrt(*(bf + i) * 1.0);
        i++;
    }
    return af;
}

int randZ(void)
{
	unsigned long long a;
	srand(time(NULL));
	a = rand() % (KEY_MAX - KET_MIN) + KET_MIN;
	for(int i = 2; i < (int)(a / 2 + 1); i++)
	{
		if (a % i == 0)
		{
			i=1;
			a = rand() % (KEY_MAX - KET_MIN) + KET_MIN;
		}
	}
	return a;
}

int *enc_k(char *bf, int a, unsigned long long *k)
{
    //printf("%llu %llu", *k, *(k + 1));
    int *af = NULL;
    af = (int*)malloc(a * sizeof(int));
    for (int i = 0; i < a; i++)
    {
        *(af + i) = poww((int)*(bf + i), *(k + 0), *(k + 1)) + i;
    }
    return af;
}

char *dec_k(int *bf, int a, unsigned long long *k)
{
    char *af = NULL;
    af = (char*)malloc(a * sizeof(char));
    
    for (int i = 0; i < a; i++)
    {
        *(af + i) = poww(*(bf + i) - i, *(k + 0), *(k + 1));
    }
    return af;
}

int poww(int a, unsigned long long b, unsigned long long c)
{
    if (b == 0) {
        return 1 % c;
    } else if (b == 1) {
        return a % c;
    }

    int t = poww(a, b / 2, c);
    t = (t * t) % c;

    if (b % 2 == 0) {
        return t;
    } else {
        return (a * t) % c;
    }
}

void keyg(void)
{
    //生成密钥
    FILE *fp1 = NULL;
    printf("生成中……\n");
	//生成p，q
    unsigned long long p, q;
	p = randZ();
	q = randZ();
	while (q == p)
		q = randZ();
    unsigned long long n = p * q;

    //生成l
	unsigned long long l;
	int i = 1;
	while(((p - 1) * i) % (q - 1) != 0) //求最小公倍数
		i++;
	l = (p - 1) * i;
	
    //生成e
    unsigned long long e = 2;
	i = 2;
	while (i < l)
	{
		if(l % i == 0 && e % i == 0) //求e与l互质
		{
			e++;
			i = 2;
			continue;
		}
		i++;
	}
            
    //生成d
	unsigned long long d = 1;
	while(e * d % l != 1)
		d++;
	//unsigned long long tmp=e*d%l;
	//printf("p=%llu\nq=%llu\nn=%llu\nl=%llu\ne=%llu\nd=%llu\n%llu\n",p,q,n,l,e,d,tmp); //e*d=1l+1
	
    //写入公钥
    fp1 = fopen("./key_pub.txt", "w");
	if (fp1 == NULL) //判断打开是否成功
	{
		printf("读取文件失败！请再次尝试\n");
		exit(1);
	}
	fprintf(fp1, "%llu %llu\n", e, n);
	fclose(fp1);
			
    // 写入私钥
	fp1 = fopen("./key.txt", "w");
	if (fp1 == NULL) //判断打开是否成功
	{
		printf("读取文件失败！请再次尝试\n");
		exit(1);
	}
	fprintf(fp1, "%llu %llu\n", d, n);
	fclose(fp1);
	printf("完成！\n\n");
}

void encall(void)
{
    //读取明文
    FILE *fp1 = NULL;
    FILE *fp2 = NULL;
    int i = 0;
    int n = 0;
    printf("请输入要加密的文件路径：\n");
    char address[ADDRESS_L];
    scanf("%s", address);
    while (getchar() != '\n')
            continue;
    fp1 = fopen(address, "r"); //打开指定文件
    if (fp1 == NULL) //判断打开是否成功
    {
        printf("读取文件失败！请再次尝试\n");
        exit(1);
    }

    //获取文件长度（不包括EOF）
    fseek(fp1, 0, SEEK_END); //定位到文件末尾
    n = ftell(fp1); //判断指针到文件开头的偏移量（即文件长度）
    rewind(fp1); //定位回文件开头

    //读取文件内容
    char *bf = (char*)malloc(n * sizeof(char));
    while (i < n)
    {
        *(bf + i) = fgetc(fp1); //复制文件字符至该内存
        i++;
    }
    fclose(fp1);
    printf("你想使用公钥加密吗？\ny. 是的\tn. 不用\n");
    char k = getchar();
    while (getchar() != '\n')
            continue;
    if(k == 'y' || k == 'Y')
    {
        //如果使用公钥加密
        unsigned long long *pk = NULL;
        pk = (unsigned long long*)malloc(2 * sizeof(unsigned long long));
        printf("请输入公钥：\n");
        scanf(" %llu %llu", (pk + 0), (pk + 1));
        while (getchar() != '\n')
            continue;
        int *af = NULL;
        af = enc_k(bf, n, pk);

        //写入密文
        fp2 = fopen("./miwen.txt", "w"); //打开指定文件
        if (fp2 == NULL) //判断打开是否成功
        {
            printf("读取文件失败！请再次尝试\n");
            exit(1);
        }

        //写入
        int i = 0;
        while (i<n)
        {
            fprintf(fp2, "%d", *(af + i));
            fputc('\0', fp2);
            i++;
        }
        fclose(fp2);

        //释放内存
        free(bf);
        free(af);
        free(pk);
        printf("完成！\n\n");

    }else{
        //不使用公钥加密
        int *af = NULL; //用于指向加密后的字符
        i = 0;
        af = enc(bf, n); //调用加密函数
        /*while (i < n)
        {
            printf("%d\0", *(af + i));
            i++;
        }*/


        //写入密文
        fp2 = fopen("./miwen.txt", "w"); //打开指定文件
        if (fp2 == NULL) //判断打开是否成功
        {
            printf("读取文件失败！请再次尝试\n");
            exit(1);
        }
            
        //写入
        i = 0;
        while (i<n)
        {
        fprintf(fp2, "%d", *(af + i));
        fputc('\0', fp2);
        i++;
        }
        fclose(fp2);

        //释放内存
        free(bf);
        free(af);
        printf("完成！\n\n");
    }
}

void decall(void)
{
    FILE *fp1 = NULL;
    FILE *fp2 = NULL;
    int i = 0;
    int n = 0;
    printf("这个文件是否使用密钥加密吗？\ny. 是的\tn. 没有\n");
    char k = getchar();
    while (getchar() != '\n')
            continue;
    if (k == 'y' || k == 'Y')
    {
        //读取密文
        printf("请输入要解密的文件路径：\n");
        char address[ADDRESS_L];
        scanf("%s", address);
        while (getchar() != '\n')
            continue;
        fp1 = fopen(address, "r"); //打开指定文件
        if (fp1 == NULL) //判断打开是否成功
        {
            printf("读取文件失败！请再次尝试\n");
            exit(1);
        }

        //获取数字个数
        int buf;
        for(n = 0; ; n++)
        {
            fscanf(fp1, "%d", &buf);
            if (fgetc(fp1) == EOF)
            {
                break;
            }
        }
        rewind(fp1);

        //读取内容
        int *bf = (int*)malloc(n * sizeof(int));
        for(i = 0; i < n; i++)
        {
            fscanf(fp1, "%llu", (bf + i));
            fgetc(fp1);
        }
        fclose(fp1);

        //使用私钥解密
        unsigned long long *pk = NULL;
        pk = (unsigned long long*)malloc(2 * sizeof(unsigned long long));
        printf("请输入私钥：\n");
        scanf(" %llu %llu", (pk + 0), (pk + 1));
        while (getchar() != '\n')
            continue;

        char* af = NULL;
        af = dec_k(bf, n, pk);

        //写入明文
        fp2 = fopen("./mingwen.txt", "w"); //打开指定文件
        if (fp2 == NULL) //判断打开是否成功
        {
            printf("读取文件失败！请再次尝试\n");
            exit(1);
        }
            
        //写入
        for (i=0; i<n; i++)
        {
            fprintf(fp2, "%c", *(af + i));
        }

        free(bf);
        free(af);
        free(pk);
        printf("完成！\n\n");
    }else{
        //读取密文
        printf("请输入要解密的文件路径：\n");
        char address[ADDRESS_L];
        scanf("%s", address);
        while (getchar() != '\n')
            continue;
        fp1 = fopen(address, "r"); //打开指定文件
        if (fp1 == NULL) //判断打开是否成功
        {
            printf("读取文件失败！请再次尝试\n");
            exit(1);
        }

        //获取数字个数
        int buf;
        for(n = 0; ; n++)
        {
            fscanf(fp1, "%d", &buf);
            if (fgetc(fp1) == EOF)
            {
                break;
            }
        }
        rewind(fp1);

        //读取内容
        int *bf = (int*)malloc(n * sizeof(int));
        for(i = 0; i < n; i++)
        {
            fscanf(fp1, "%d", (bf + i));
            fgetc(fp1);
        }
        fclose(fp1);
        
        //不使用私钥解密
        char *af = NULL;
        af = dec(bf, n);


        //写入明文
        fp2 = fopen("./mingwen.txt", "w"); //打开指定文件
        if (fp2 == NULL) //判断打开是否成功
        {
            printf("读取文件失败！请再次尝试\n");
            exit(1);
        }
    
        //写入
        for (i=0; i<n; i++)
        {
            fprintf(fp2, "%c", *(af + i));
        }

        free(bf);
        free(af);
        printf("完成！\n\n");
    }
}