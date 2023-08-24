#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int *enc(char*, int); //用于加密操作的函数
char *dec(int*, int); //用于解密操作的函数
int randZ(void); //获取质数
int *enc_k(char*, int, unsigned long long*);
char *dec_k(int*, int, unsigned long long*);
int poww(int, unsigned long long, unsigned long long);

int main (int argc, char *argv[]) //加密文本文件
{
    FILE *fp1 = NULL;
    FILE *fp2 = NULL;
    int i=0;
    int n=0;

    if (argc == 2)
    {
        //如果只有一个参数（除本身）
        if (strcmp(argv[1], "-h") == 0)
        {
            //如果参数是“-h”
            printf("Easy Text Encryption Tool\tvesion 0.3.0\n"
                   "usage:\tetet [OPTION] [FILE]\n"
                   "OPTION:\t-e encryption\n"
                   "\t-d decrypt\n"
                   "\t-k generate key\n"
                   "You can feel free to use and modify the source code don't need my allow, and I do not reserve the right of authorship. \n"); //打印使用帮助
            exit(0); 
            //正常退出

        }else if (strcmp(argv[1], "-k") == 0)
        {
            //生成密钥
            printf("Key generating...\n");
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
            unsigned long long e = rand() % l-2 + 2;
			i = 2;
			while (i < l)
			{
				if(l % i == 0 && e % i == 0) //求e与l互质
				{
					e = rand() % l-2 + 2;
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
            fp1 = fopen("./key.pub", "w");
			if (fp1 == NULL) //判断打开是否成功
			{
				printf("Faild! Please try again...\n");
				exit(1);
			}
			fprintf(fp1, "%llu %llu\n", e, n);
			fclose(fp1);
			
            // 写入私钥
			fp1 = fopen("./key", "w");
			if (fp1 == NULL) //判断打开是否成功
			{
				printf("Faild! Please try again...\n");
				exit(1);
			}
			fprintf(fp1, "%llu %llu\n", d, n);
			fclose(fp1);
			printf("Done! \n");

        }else{
            printf("Try \"etet -h\" for more information.\n"); //错误提示
            exit(1); //异常退出
        }

    }else if (argc>2) //多于一个参数
    {

        if (strcmp(argv[1], "-e") == 0) //使用“-e”加密选项
        {
            //读取明文
            fp1 = fopen(argv[2], "r"); //打开指定文件
            if (fp1 == NULL) //判断打开是否成功
            {
                printf("Faild! Please try again...\n");
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
            printf("Do you want to use a key? [N or y] ");
            char k = getchar();
            if(k == 'y' || k == 'Y')
            {
                //如果使用公钥加密
                unsigned long long *pk = NULL;
                pk = (unsigned long long*)malloc(2 * sizeof(unsigned long long));
                printf("Please enter the public key:\n");
                scanf(" %llu %llu", (pk + 0), (pk + 1));
                int *af = NULL;
                af = enc_k(bf, n, pk);

                //写入密文
                fp2 = fopen("./密文", "w"); //打开指定文件
                if (fp2 == NULL) //判断打开是否成功
                {
                    printf("Faild! Please try again...\n");
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
                free(pk);
                printf("Done! \n");

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
                fp2 = fopen("./密文", "w"); //打开指定文件
                if (fp2 == NULL) //判断打开是否成功
                {
                    printf("Faild! Please try again...\n");
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
                printf("Done! \n");
            }
            
            
            
        }else if (strcmp(argv[1], "-d") == 0) //如果使用“-d”解密选项
        {
            
            printf("Does it use a key? [N or y] ");
            char k = getchar();
            if (k == 'y' || k == 'Y')
            {
                //读取密文
                fp1 = fopen(argv[2], "r"); //打开指定文件
                if (fp1 == NULL) //判断打开是否成功
                {
                    printf("Faild! Please try again...\n");
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
                printf("Please enter your private key: \n");
                scanf(" %llu %llu", (pk + 0), (pk + 1));

                char* af = NULL;
                af = dec_k(bf, n, pk);

                //写入明文
                fp2 = fopen("./明文", "w"); //打开指定文件
                if (fp2 == NULL) //判断打开是否成功
                {
                    printf("Faild! Please try again...\n");
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
                printf("Done! \n");
            }else{
                //读取密文
                fp1 = fopen(argv[2], "r"); //打开指定文件
                if (fp1 == NULL) //判断打开是否成功
                {
                    printf("Faild! Please try again...\n");
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
                fp2 = fopen("./明文", "w"); //打开指定文件
                if (fp2 == NULL) //判断打开是否成功
                {
                    printf("Faild! Please try again...\n");
                    exit(1);
                }
            
                //写入
                for (i=0; i<n; i++)
                {
                    fprintf(fp2, "%c", *(af + i));
                }

                free(bf);
                free(af);
                printf("Done! \n");
            }
            


        }else{
            printf("Try \"etet -h\" for more information.\n"); //错误提示
            exit(1); //异常退出
        }

    }else{
        system("open \"https://www.bilibili.com/video/BV1GJ411x7h7/?share_source=copy_web&vd_source=a2705d503a7fba8db2b5b70abafe87ed\"");
        printf("Try \"etet -h\" for more information.\n"); //错误提示
        exit(1); //异常退出
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
	a = rand() % (200 - 10) + 10;
	for(int i = 2; i < (int)(a / 2 + 1); i++)
	{
		if (a % i == 0)
		{
			i=1;
			a = rand() % (200 - 20) + 20;
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
        *(af + i) = poww((int)*(bf + i), *(k + 0), *(k + 1));
    }
    return af;
}

char *dec_k(int *bf, int a, unsigned long long *k)
{
    char *af = NULL;
    af = (char*)malloc(a * sizeof(char));
    
    for (int i = 0; i < a; i++)
    {
        *(af + i) = poww(*(bf + i), *(k + 0), *(k + 1));
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