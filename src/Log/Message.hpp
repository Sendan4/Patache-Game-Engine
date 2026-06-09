#ifndef MESSAGE_HPP
#define MESSAGE_HPP

namespace Patache
{
void FatalErrorMessage (const char * const, const char * const, const Patache::Config &);
void ErrorMessage (const char * const);
void WarningMessage (const char * const);
}

#endif
