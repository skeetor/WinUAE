#include "patterns/serialize.h"

void checkException(bool Throw, const wxString &msg, const wxString &key, const wxString &value)
{
	if (!Throw)
		return;

	throw SerializeException(msg.ToStdString(), key.ToStdString(), value.ToStdString());
}
