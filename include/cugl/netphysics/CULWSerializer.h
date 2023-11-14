//
//  CULWSerializer.h
//  Networked Physics Library
// 
//  This class provides a lightweight serializer for networked physics. It
//  removed the type safety of the Serializer class, and relies on the user
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

#ifndef __CU_LW_SERIALIZER_H__
#define __CU_LW_SERIALIZER_H__

#include <vector>
#include <memory>
#include <SDL_stdinc.h>
#include <cugl/base/CUEndian.h>

namespace cugl {

    /**
     * The CUGL networked physics classes.
     *
     * This internal namespace is for optional networking physics management package.
     * This package provides automatic synchronization of physics objects across devices.
     */
    namespace netphysics {

class LWSerializer{
private:
    /** The buffered serialized data */
    std::vector<std::byte> _data;
    
public:
    /** 
     * Constructor LWSerializer, no initialization required. 
     */
    LWSerializer() {}

    /** 
     * Allocates a new LWSerializer.
     */
    static std::shared_ptr<LWSerializer> alloc() {
        return std::make_shared<LWSerializer>();
    }
    
    /**
	 * Writes a boolean to the input buffer.
	 *
	 * @param b The boolean to write
	 */
    void writeBool(bool b){
        _data.push_back(b ? std::byte(1) : std::byte(0));
    }
    
    /**
    * Writes a byte to the input buffer.
    * 
    * @param b The byte to write
    */
    void writeByte(std::byte b){
        _data.push_back(b);
    }
    
    /**
	 * Writes a byte vector to the input buffer.
	 *
	 * @param v The byte vector to write
	 */
    void writeByteVector(const std::vector<std::byte>& v){
        _data.insert(_data.end(), v.begin(), v.end());
    }
    
    /**
    * Rewrites the first four bytes of the input buffer with the given Uint32.
    *   
    * Requires the input buffer has at least four bytes.
    * 
    * @param i The byte vector to write
    */
    void rewriteFirstUint32(Uint32 i){
        Uint32 ii = marshall(i);
        const std::byte* bytes = reinterpret_cast<const std::byte*>(&ii);
        for (size_t j = 0; j < sizeof(Uint32); j++) {
            _data[j] = bytes[j];
        }
    }
    
    /**
     * Writes a float to the input buffer.
     *
     * @param f The float to write
     */
    void writeFloat(float f){
        float ii = marshall(f);
        const std::byte* bytes = reinterpret_cast<const std::byte*>(&ii);
        for (size_t j = 0; j < sizeof(float); j++) {
            _data.push_back(bytes[j]);
        }
    }
    
    /**
	 * Writes a Signed 32-bit integer to the input buffer.
	 *
	 * @param i The Sint32 to write
	 */
    void writeSint32(Sint32 i){
        Sint32 ii = marshall(i);
        const std::byte* bytes = reinterpret_cast<const std::byte*>(&ii);
        for (size_t j = 0; j < sizeof(Sint32); j++) {
            _data.push_back(bytes[j]);
        }
    }
    
    /**
     * Writes a Unsigned 16-bit integer to the input buffer.
     *
     * @param i The Uint16 to write
     */
    void writeUint16(Uint16 i){
        Uint16 ii = marshall(i);
        const std::byte* bytes = reinterpret_cast<const std::byte*>(&ii);
        for (size_t j = 0; j < sizeof(Uint16); j++) {
            _data.push_back(bytes[j]);
        }
    }
    
    /**
     * Writes a Unsigned 32-bit integer to the input buffer.
     *
     * @param i the unsigned Uint32 to write
     */
    void writeUint32(Uint32 i){
        Uint32 ii = marshall(i);
        const std::byte* bytes = reinterpret_cast<const std::byte*>(&ii);
        for (size_t j = 0; j < sizeof(Uint32); j++) {
            _data.push_back(bytes[j]);
        }
    }
    
    /**
	 * Writes a Unsigned 64-bit integer to the input buffer.
	 *
	 * @param i the unsigned Uint64 to write
	 */
    void writeUint64(Uint64 i){
        Uint64 ii = marshall(i);
        const std::byte* bytes = reinterpret_cast<const std::byte*>(&ii);
        for (size_t j = 0; j < sizeof(Uint64); j++) {
            _data.push_back(bytes[j]);
        }
    }
    
    /** 
     * Returns the serialized data.
     * 
     * @return A const reference to the serialized data. (Will be lost if reset)
     */
    const std::vector<std::byte>& serialize() {
        return _data;
    }

    /**
     * Clears the input buffer.
     * 
     * !! This will make previous serialize() returns invalid !!
     */
    void reset() {
        _data.clear();
    }
};

    }
}

#endif /* __CU_LW_SERIALIZER_H__ */
