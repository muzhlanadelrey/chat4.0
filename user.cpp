#include <string>
#include "user.h"
using namespace std;

string& User::getUserLogin()
{
	return _login;
}

void User::setUserLogin(const string& login)
{
	_login = login;
}

string& User::getUserName()
{
	return _name;
}

void User::setUserName(const string& name)
{
	_name = name;
}

string& User::getUserPassword()
{
	return _password;
}

void User::setUserPassword(const string& password)
{
	_password = password;
}