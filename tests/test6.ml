
if(-5 != -5){
    print(-5, "es diferente que", -5);
}else{
    print(-5, "no es diferente que", -5);
}

function is_less_than(a, b){
    if(a < b){
        print(a, "es menor que", b);
    }else{
        print(a, "no es menor que", b);
    }
}

is_less_than(1, 1);
is_less_than(2, 1.999);
is_less_than(3, 3.001);
is_less_than(4, 3.999);
is_less_than(5, 5.001);
