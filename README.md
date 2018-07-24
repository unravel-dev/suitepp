### suitepp - c++11 header only unit testing library

#No dependecnies 1 small header only

#Provides a simple api and a semi-decent output that does the job.

```C++
int main()
{
	suitepp::test("test1", []()
	{
		int a = 12;
		EXPECT(a == 12);
		EXPECT(a <= 15);
		auto expression = []()
		{
			//some code
		};
		EXPECT(NOTHROWS(expression()));
		EXPECT(THROWS(expression()));
		EXPECT(THROWS_AS(expression(), std::exception));
	});
	return 0;
}
```

would output something like
```C++
-------------- test1 --------------
[ OK ] check (1) (0.0463ms)
[FAIL] check (2) (0.0437ms) at D:\Workspace\Repositories\suitepp\suitepp\tests\tests\main.cpp:13
       a > 15
       12 > 15

[ OK ] check (3) (0.031ms)
[FAIL] check (4) (0.0326ms) at D:\Workspace\Repositories\suitepp\suitepp\tests\tests\main.cpp:15
       (a < 15) && (a == 10)
       true && false

[ OK ] check (5) (0.0129ms)
[FAIL] check (6) (0.0141ms) at D:\Workspace\Repositories\suitepp\suitepp\tests\tests\main.cpp:22
       [&]() { try { expression(); } catch(...) { return true; } return false; }()
       false

[FAIL] check (7) (0.0137ms) at D:\Workspace\Repositories\suitepp\suitepp\tests\tests\main.cpp:23
       [&]() { try { expression(); } catch(const std::exception&) { return true; } catch(...) { } return false; }()
       false

[FAIL] check (total) (3.1737ms) at d:\workspace\repositories\suitepp\suitepp\tests\tests\suitepp/suitepp.hpp:354
       whole_case()
       false
	   
[FAIL]  Failure! 5/8 checks failed :(
```
