#ifndef AUTOPLACE_GLOBAL_H
#define AUTOPLACE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(AUTOPLACE_LIBRARY)
#  define AUTOPLACE_EXPORT Q_DECL_EXPORT
#else
#  define AUTOPLACE_EXPORT Q_DECL_IMPORT
#endif

#endif // AUTOPLACE_GLOBAL_H
