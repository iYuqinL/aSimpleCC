int func(int a);

int func(int a)
{
 return 1;
}

int main()
{
    int i=0;
    func(i);
    func(1);
    if(i)
    {
        func(i);
    }
}
