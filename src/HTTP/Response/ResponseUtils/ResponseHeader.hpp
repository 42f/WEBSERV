#pragma once

# include <sstream>
# include <iostream>
# include <string>
# include <map>
# include <utility>

namespace headerTitle {

	enum Title {
		Allow,
		Content_Language,
		Content_Length,
		Content_Location,
		Content_Type,
		Date,
		Last_Modified,
		Location,
		Retry_After,
		Server,
		Transfer_Encoding,
		WWW_Authenticate
	};
	class HeaderTitleField {

		public:
			static std::string& get(Title title);

		private:
			std::map<Title, std::string>	_fields;
			static HeaderTitleField			_instance;

			HeaderTitleField();
	};
}
