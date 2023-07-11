#include <string>
#include "message.h"
using namespace std;

string Message::getMessage() const
{
	return _message;
}

void Message::setMessage(const string& message)
{
	_message = message;
}

string Message::getSender() const
{
	return _sender;
}

void Message::setSender(const string& sender)
{
	_sender = sender;
}

string Message::getRecipient() const
{
	return _recipient;
}

void Message::setRecipient(const string& recipient)
{
	_recipient = recipient;
}
