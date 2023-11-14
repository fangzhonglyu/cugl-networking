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

class LWDeserializer{
private:
    /** Currently loaded data */
    std::vector<std::byte> _data;
    /** Position in the data of next byte to read */
    size_t _pos;

public:
    LWDeserializer() : _pos(0) {}
    
    static std::shared_ptr<LWDeserializer> alloc() {
        return std::make_shared<LWDeserializer>();
    }
    
    void receive(const std::vector<std::byte>& msg){
        _data = msg;
        _pos = 0;
    }
    
    bool readBool(){
        if (_pos >= _data.size()) {
            return false;
        }
        uint8_t value = static_cast<uint8_t>(_data[_pos++]);
        return value == 1;
    }
    
    std::byte readByte(){
        if (_pos >= _data.size()) {
            return std::byte(0);
        }
        const std::byte b = _data[_pos++];
        return b;
    }
    
    float readFloat(){
        if (_pos >= _data.size()) {
            return 0.0f;
        }
        const float* r = reinterpret_cast<const float*>(_data.data() + _pos);
        _pos += sizeof(float);
        return marshall(*r);
    }
    
    Sint32 readSint32(){
        if (_pos >= _data.size()) {
            return 0;
        }
        const Sint32* r = reinterpret_cast<const Sint32*>(_data.data() + _pos);
        _pos += sizeof(Sint32);
        return marshall(*r);
    }
    
    Uint16 readUint16(){
        if (_pos >= _data.size()) {
            return 0;
        }
        const Uint16* r = reinterpret_cast<const Uint16*>(_data.data() + _pos);
        _pos += sizeof(Uint16);
        return marshall(*r);
    }
    
    Uint32 readUint32(){
        if (_pos >= _data.size()) {
            return 0;
        }
        const Uint32* r = reinterpret_cast<const Uint32*>(_data.data() + _pos);
        _pos += sizeof(Uint32);
        return marshall(*r);
    }
    
    Uint64 readUint64(){
        if (_pos >= _data.size()) {
            return 0;
        }
        const Uint64* r = reinterpret_cast<const Uint64*>(_data.data() + _pos);
        _pos += sizeof(Uint64);
        return marshall(*r);
    }
    
    void reset(){
        _pos = 0;
        _data.clear();
    }
    
};

    }
}

#endif /* __CU_LW_SERIALIZER_H__ */
