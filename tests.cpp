#include <iostream>
using namespace std;

int main()
{
    string t = "20", h = "30", ac_status = "0";

    string JSON_Data = "{\"temp\":";
    JSON_Data += t;
    JSON_Data += ",\"hum\":";
    JSON_Data += h;
    JSON_Data += ",\"ac\":";
    JSON_Data += ac_status;
    JSON_Data += "}";

    // printf("%s", JSON_Data);
    cout << JSON_Data;

    return 0;
}