
struct data_t
{
    int a;
    int b;
};

struct ctrl_t
{
    int a;
    struct data_t d;
};

int func()
{
    return 0;
}

int main()
{
    int i=0;
    struct ctrl_t control;
    struct ctrl_t c1;

    control.a = 0;
    control.d.a = 1;
    func();
    return 0;
}

