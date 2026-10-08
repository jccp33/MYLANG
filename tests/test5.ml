function my_function(name, last){
    print("hello", name, last);
}

function print_total(price, quantity){
    var total = price * quantity;
    print("TOTAL: ", total);
}

my_function("manuel", "lopez");
my_function("jose", "ramirez");
my_function("julian", "corrales");
my_function("jaziel", 33);

print_total(9.99, 10);
