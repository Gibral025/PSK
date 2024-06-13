#include <stdio.h>

int main() {
    int i;  
    int sum = 0; 
    int n;  

    printf("Masukkan jumlah angka yang akan dijumlahkan: ");
    scanf("%d", &n);

    for(i = 1; i <= n; i++) {
        if(i % 2 == 0) { 
            sum += i;  
        } else {
            printf("%d adalah angka ganjil dan tidak dijumlahkan\n", i);
        }
    }

    printf("Jumlah dari semua angka genap antara 1 hingga %d adalah %d\n", n, sum);

    return 0;
}
