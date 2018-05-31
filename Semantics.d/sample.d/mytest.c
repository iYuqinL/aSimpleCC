int func(int a);



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

