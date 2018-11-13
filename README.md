# suitepp - c++11 header only unit testing library

### No dependecnies, just 1 small header only

### Provides a simple api and a semi-decent output that does the job.

```C++
int main()
{
    suitepp::test("test1", []()
    {
        int a = 12;
        EXPECT(a == 12);
        EXPECT(a >= 15);
        auto expression = []()
        {
            //some code
        };
        EXPECT_NOTHROWS(expression());
        EXPECT_THROWS(expression());
        EXPECT_THROWS_AS(expression(), std::exception);
    });
    return 0;
}
```

would output something like
```C++
-----------
[[ test1 ]]
-----------
[ OK ] check a == 12 (0.0765ms) 
[FAIL] check a >= 15 (0.0013ms) at D:/Workspace/Repositories/dynopp/dynopp/tests/main.cpp:112
       12 >= 15

[ OK ] check expression() (0.0004ms) 
[FAIL] check expression() (0.0004ms) at D:/Workspace/Repositories/dynopp/dynopp/tests/main.cpp:118
       expression() expected to throw but did not

[FAIL] check expression() (0.0004ms) at D:/Workspace/Repositories/dynopp/dynopp/tests/main.cpp:119
       expression() expected to throw std::exception but did not

-----------

[FAIL]  Failure! 3/5 checks failed :(
```
