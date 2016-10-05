#include <stdio.h>

#define TRUE 1
#define FALSE 0

int main(int argc, char *argv[])
{
	FILE *fp;
	FILE *fp2;
	unsigned char is_kernel = FALSE;
	char buf[80];   
        char str1[20], str2[20];


	fp = fopen("prog.c","w");
	fp2 = fopen("kconf.h","r");

	if (fp2 == NULL)
	{
		fclose(fp);
		printf("Error : kconf.h does not exist!");
		exit(1);
	}

	if (fp == NULL)
	{
		printf("Error : prog.c already exists!");
		exit(1);
	}

	// scan kconf.h and see what's there
        while (fgets(buf, 80, fp2) != NULL)
        {
                // eat up for #if defined ... #endif
                if (strncmp(buf, "#if ", 4) == 0)
                {
                        do
                        {
                                fgets(buf, 80, fp2);
                        }
                        while (strncmp(buf, "#endif", 6) != 0);
                }

                if (strncmp(buf, "#define", 7) == 0)
                {
                        sscanf(buf, "%s %s", str1, str2);

                        if (strcmp(str2, "KERNEL_M") == 0)
                        {
                                is_kernel = TRUE;
                        }
		}
	}


	fprintf(fp, "//========================================================================\n");
	fprintf(fp, "// File\t\t: prog.c \n");
	fprintf(fp, "// Author\t: your_name \n");
	fprintf(fp, "// Date\t\t: 2006. 10. 01 \n");
	fprintf(fp, "// Description\t: Nano OS Application \n");
	fprintf(fp, "//========================================================================\n");
	fprintf(fp, "// Copyright 2004-2010. \n");
	fprintf(fp, "//========================================================================\n");
	fprintf(fp, "\n");
	fprintf(fp, "#include \"nos.h\"\n");
	fprintf(fp, "\n");
if (is_kernel) 
{
	fprintf(fp, "void task1(void *); // for thread1 \n");
	fprintf(fp, "void task2(void *); // for thread2 \n");
	fprintf(fp, "\n");
	fprintf(fp, "void task1(void *args)\n");
	fprintf(fp, "{\n");
	fprintf(fp, "\twhile (1)\n");
	fprintf(fp, "\t{\n");
	fprintf(fp, "\t\t/////////////////////////////////\n");
	fprintf(fp, "\t\t// Your Task1 for Thread1 Here //\n");
	fprintf(fp, "\t\t/////////////////////////////////\n");
	fprintf(fp, "\t}\n");
	fprintf(fp, "}\n");
	fprintf(fp, "\n");	

	fprintf(fp, "void task2(void *args)\n");
	fprintf(fp, "{\n");
	fprintf(fp, "\twhile (1)\n");
	fprintf(fp, "\t{\n");
	fprintf(fp, "\t\t/////////////////////////////////\n");
	fprintf(fp, "\t\t// Your Task2 for Thread2 Here //\n");
	fprintf(fp, "\t\t/////////////////////////////////\n");
	fprintf(fp, "\t}\n");
	fprintf(fp, "}\n");
	fprintf(fp, "\n");

}
	fprintf(fp, "int main(void)\n");
	fprintf(fp, "{\n");
	fprintf(fp, "\tnos_init();\n");
	fprintf(fp, "\n");
	fprintf(fp, "\t/////////////////////////\n");
	fprintf(fp, "\t// Your Main Code Here //\n");
	fprintf(fp, "\t/////////////////////////\n");
	fprintf(fp, "\n");
if (is_kernel)
{
	fprintf(fp, "\tthread_create(1, task1, NULL, DEFAULT_STACK_SIZE, PRIORITY_NORMAL);\n");
	fprintf(fp, "\tthread_create(2, task2, NULL, DEFAULT_STACK_SIZE, PRIORITY_NORMAL);\n");
	fprintf(fp, "\n");
	fprintf(fp, "\tnos_sched_start();\n");
	fprintf(fp, "\n");
}
	fprintf(fp, "\treturn 0;\n");
	fprintf(fp, "}\n");

	fclose(fp);

	return 0;
}
