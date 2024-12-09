#include "storage/storage/layers.h"
/**
 * @brief 新建一个layer在overlay文件中
 * 
 * @param id 
 * @param parentLayer 
 * @param mountLabel 
 * @param options 
 * @param writeable 
 * @param diff 
 * @return std::tuple<std::shared_ptr<Layer>,int64_t> 
 */
std::tuple<std::shared_ptr<Layer>,int64_t> layerStore::create(
        std::string& id,std::shared_ptr<Layer> parentLayer,std::vector<std::string>names,
        std::string mountLabel,std::map<std::string,std::string> options,
        std::shared_ptr<LayerOptions>moreOptions,bool writeable,
        std::ifstream& diff)
{
// 1. GenerateRandomID生成随机ID，判断id没有重复

// 2. 定义相关变量
    // 	var (
	// 	templateIDMappings         *idtools.IDMappings
	// 	templateMetadata           string
	// 	templateCompressedDigest   digest.Digest
	// 	templateCompressedSize     int64
	// 	templateUncompressedDigest digest.Digest
	// 	templateTOCDigest          digest.Digest
	// 	templateUncompressedSize   int64
	// 	templateCompressionType    archive.Compression
	// 	templateUIDs, templateGIDs []uint32
	// 	templateTSdata             []byte
	// )
// 3. 创建layer

// 4. r.byid[id] = layer//在store中添加

// 5. 创建id对应的laryer层文件夹
	// opts := drivers.CreateOpts{
	// 	MountLabel: mountLabel,
	// 	StorageOpt: options,
	// 	IDMappings: idMappings,
	// }
    // err = r.driver.CreateReadWrite(id, parent, &opts); 

    return std::make_tuple(nullptr,0);
}
/**
 * @brief 加载layer
 * 
 * @param lockedForWriting 
 * @return true 
 * @return false 
 */
bool layerStore::load(bool lockedForWriting){
    //比较复杂
    return false;
}