#include "image/types/define/namespace.h"
/**
 * @brief 添加或替换命名空间选项
 * 
 * @param options 要添加的命名空间选项列表
 */
void NamespaceOptions::AddOrReplace(vector<NamespaceOption> options){
	for (auto const& option : options) {
		// auto it = std::find_if(this->val.begin(),this->val.end(),
		// 	[&option]( auto const& existing) { 
		// 		return existing.Name == option.Name;
		// 	});
		// if (it != this->val.end()) {
		// 	*it = option;
		// } else {
		// 	this->val.emplace_back(option);
		// }
        this->val.emplace_back(option);
	}
}
