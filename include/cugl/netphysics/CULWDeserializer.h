//
//  CULWDeserializer.h
//  Networked Physics Library
// 
//  This class provides a lightweight deserializer for networked physics. It
//  removed the type safety of the Deserializer class, and relies on the user
//  to know the type of the data. However, it is a more space efficient
//  serializer, and is more appropriate for networked physics.
//
//  CUGL MIT License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author: Barry Lyu
//  Version: 11/13/23
//

#ifndef __CU_LW_DESERIALIZER_H__
#define __CU_LW_DESERIALIZER_H__

#include <vector>

namespace cugl {

    /**
     * The CUGL networked physics classes.
     *
     * This internal namespace is for optional networking physics management package.
     * This package provides automatic synchronization of physics objects across devices.
     */
    namespace netphysics {

class LWDeserializer{
private:
    /** Currently loaded data */
    std::vector<std::byte> _data;
    /** Position in the data of next byte to read */
    size_t _pos;

public:
    /**
     * Constructor LWDeserializer, no initialization required.
     */
    LWDeserializer() : _pos(0) {}
    
    /**
     * Allocates a new LWDeserializer.
     */
    static std::shared_ptr<LWDeserializer> alloc() {
        return std::make_shared<LWDeserializer>();
    }
    
    /**
	 * Loads a byte vector into the deserializer.
	 */
    void receive(const std::vector<std::byte>& msg){
        _data = msg;
        _pos = 0;
    }
    
    /**
     * Reads a boolean from the loaded byte vector.
     */
    bool readBool(){
        if (_pos >= _data.size()) {
            return false;
        }
        uint8_t value = static_cast<uint8_t>(_data[_pos++]);
        return value == 1;
    }
    
    /**
     * Reads a byte from the loaded byte vector.
     */
    std::byte readByte(){
        if (_pos >= _data.size()) {
            return std::byte(0);
        }
        const std::byte b = _data[_pos++];
        return b;
    }
    
    /**
     * Reads a float from the loaded byte vector.
     */
    float readFloat(){
        if (_pos >= _data.size()) {
            return 0.0f;
        }
        const float* r = reinterpret_cast<const float*>(_data.data() + _pos);
        _pos += sizeof(float);
        return marshall(*r);
    }
    
    /**
     * Reads a Sint32 from the loaded byte vector.
     */
    Sint32 readSint32(){
        if (_pos >= _data.size()) {
            return 0;
        }
        const Sint32* r = reinterpret_cast<const Sint32*>(_data.data() + _pos);
        _pos += sizeof(Sint32);
        return marshall(*r);
    }
    
    /**
     * Reads a Uint16 from the loaded byte vector.
     */
    Uint16 readUint16(){
        if (_pos >= _data.size()) {
            return 0;
        }
        const Uint16* r = reinterpret_cast<const Uint16*>(_data.data() + _pos);
        _pos += sizeof(Uint16);
        return marshall(*r);
    }
    
    /**
     * Reads a Uint32 from the loaded byte vector.
     */
    Uint32 readUint32(){
        if (_pos >= _data.size()) {
            return 0;
        }
        const Uint32* r = reinterpret_cast<const Uint32*>(_data.data() + _pos);
        _pos += sizeof(Uint32);
        return marshall(*r);
    }
    
    /**
     * Reads a Uint64 from the loaded byte vector.
     */
    Uint64 readUint64(){
        if (_pos >= _data.size()) {
            return 0;
        }
        const Uint64* r = reinterpret_cast<const Uint64*>(_data.data() + _pos);
        _pos += sizeof(Uint64);
        return marshall(*r);
    }
    
    /**
	 * Resets the deserializer and clears the loaded byte vector.
	 */
    void reset(){
        _pos = 0;
        _data.clear();
    }
    
};

    }
}
#endif /* __CU_LW_DESERIALIZER_H__ */
