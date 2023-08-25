#pragma once

#ifdef RENDERER__EXPORTS
#define RENDERER_API   __declspec( dllexport )
#else
#define RENDERER_API   __declspec( dllimport )
#endif

#pragma warning(disable : 4251)
#pragma warning(disable : 4251)