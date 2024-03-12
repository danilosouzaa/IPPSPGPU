#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

int par_impar(int num);
char* filtro_strings(char str[], int espacoUnderline, int quebraDeLinha);
char letraMaiuscula( char *str, int index );
int fileExists(char fileName[]);
int verificador_de_tipo2(float num);
int floatcmp2(float num1, float num2, int casasDecimais);
int floatcmp(float num1, float num2);
int doublecmp(double num1, double num2);
char charMaiusculo( char c );
char* stringMaiusculo( char *str );

#endif // UTIL_H_INCLUDED
