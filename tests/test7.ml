
var cont = 0;
while(cont < 5){
    print("cont: ", cont);
    cont = cont + 1;
}

print("---------------------------------------");

function print_counter(c){
    var cont = 0;
    while(cont < c){
        print("cont: ", cont);
        cont = cont + 1;
    }
}

print_counter(15);

print("---------------------------------------");

function factorial_print(n, res) {
    if (n == 0) {
        print("Factorial:", res);
    } else {
        factorial_print(n - 1, res * n);
    }
}

print("Calculando factorial de 5 recursivamente...");
factorial_print(5, 1);
