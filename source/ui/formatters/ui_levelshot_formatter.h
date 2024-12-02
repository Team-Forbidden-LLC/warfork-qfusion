#ifndef __UI_LEVELSHOT_FORMATTER_H__
#define __UI_LEVELSHOT_FORMATTER_H__

#include <Rocket/Controls/DataFormatter.h>

namespace WSWUI
{
	/// Converts a map's short name into an rml with the appropriate
	/// levelshot.
	class LevelShotFormatter : public Rocket::Controls::DataFormatter
	{
	public:
		LevelShotFormatter() : Rocket::Controls::DataFormatter("levelshot"){}

		void FormatData(Rml::String& formatted_data, const Rml::StringList& raw_data)
		{
			// NOTE: we don't check for unescaped quotes in raw_data... probably
			// we should.
			for(Rml::StringList::const_iterator it = raw_data.begin(); it != raw_data.end(); ++it){
				formatted_data += " <levelshot src = \"" + (*it) + "\"/>";
			}
		}
	};
}

#endif // __UI_LEVELSHOT_FORMATTER_H__
