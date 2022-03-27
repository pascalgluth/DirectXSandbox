#pragma once

#include <fmt/format.h>

#ifdef _DEBUG

#define FILE_MODEL(str) fmt::format("../bin/Content/Models/{}", str)
#define FILE_TEXTURE(str) fmt::format("../bin/Content/Textures/{}", str)
#define FILE_OTHER(str) fmt::format("../bin/Content/Other/{}", str)
#define CONTENT_DIR "../bin/Content/"
#define MODEL_DIR "../bin/Content/Models/"
#define TEXTURE_DIR "../bin/Content/Textures/"
#define OTHER_DIR "../bin/Content/Other/"
#define CONTENT(str) fmt::format("../bin/Content/{}", str)

#else
#endif