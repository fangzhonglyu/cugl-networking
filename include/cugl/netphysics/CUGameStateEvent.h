//
//  CUGameStateEvent.h
//  Networked Physics Library
//
//  This class represents an event of a game state change.
//  It is handled by the NetEventController internally.
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

#ifndef __CU_GAME_STATE_EVENT_H__
#define __CU_GAME_STATE_EVENT_H__

#include <cugl/netphysics/CUNetEvent.h>
#include <cugl/util/CUDebug.h>

namespace cugl {

    /**
     * The CUGL networked physics classes.
     *
     * This internal namespace is for optional networking physics management package.
     * This package provides automatic synchronization of physics objects across devices.
     */
    namespace netphysics {

        /**
         * This class represents a message for the networked physics library to notify of game state changes, such as start game, reset, or pause.
         */
        class GameStateEvent : public NetEvent {
        public:
            enum Type
            {
                UID_ASSIGN = 100, 
                CLIENT_RDY = 101,
                GAME_START = 102, 
                GAME_RESET = 103, // Not used
                GAME_PAUSE = 104, // Not used
                GAME_RESUME = 105 // Not used
            };
        protected:
            /** Internal type of the game state message */
            Type _type;
            Uint32 _shortUID;

        public:
            /** 
             *  Returns a newly allocated empty game state event
			 */
            static std::shared_ptr<GameStateEvent> alloc() {
                return std::make_shared<GameStateEvent>();
            }

            /***
			 *  Returns a newly allocated empty NetEvent
			 */
            std::shared_ptr<NetEvent> newEvent() override {
                return std::make_shared<GameStateEvent>();
            }

            /**
             *  Returns a newly allocated event for broadcasting game start
             */
            static std::shared_ptr<NetEvent> allocGameStart() {
                std::shared_ptr<GameStateEvent> ptr = std::make_shared<GameStateEvent>();
                ptr->setType(GAME_START);
                return ptr;
            }

            /**
             *  Returns a newly allocated event for marking the current client as ready
             */
            static std::shared_ptr<NetEvent> allocReady() {
                std::shared_ptr<GameStateEvent> ptr = std::make_shared<GameStateEvent>();
                ptr->setType(CLIENT_RDY);
                return ptr;
            }

            /**
             *  Returns a newly allocated event for assigning UID for clients
             *  
             *  Host to 1 client only. Not meant to be broadcasted.
             * 
             *  @param _shortUID The short UID to assign
             */
            static std::shared_ptr<NetEvent> allocUIDAssign(Uint32 _shortUID) {
                std::shared_ptr<GameStateEvent> ptr = std::make_shared<GameStateEvent>();
                ptr->setType(UID_ASSIGN);
                ptr->_shortUID = _shortUID;
                return ptr;
            }

            /**
			 *  Returns a newly allocated event for broadcasting game start
			 */
            GameStateEvent() {
                _type = GAME_START;
            }

            /**
             *  Constructs a newly allocated event
             * 
             *  @param t The type of the event
             */
            GameStateEvent(Type t) {
                _type = t;
            }

            /**
			 *  Constructs a newly allocated event
			 * 
			 *  @param t The type of the event
			 *  @param _shortUID The short UID to assign
			 */
            void setType(Type t) {
                _type = t;
            }

            /**
			 *  Returns the type of the event
			 */
            Type getType() const {
                return _type;
            }

            /**
             *  Returns the shortUID of the event
             *  
             *  If the event is not UID_ASSIGN, this method returns 0.
             *  Valid shortUIDs are guaranteed to be greater than 0.
             */
            Uint8 getShortUID() const {
                return _shortUID;
            }

            /**
             *  Serializes all information in the event to a byte vector.
             */
            std::vector<std::byte> serialize() override {
                std::vector<std::byte> data;
                switch (_type) {
                case GAME_START:
                    data.push_back(std::byte(GAME_START));
                    break;
                case GAME_RESET:
                    data.push_back(std::byte(GAME_RESET));
                    break;
                case GAME_PAUSE:
                    data.push_back(std::byte(GAME_PAUSE));
                    break;
                case GAME_RESUME:
                    data.push_back(std::byte(GAME_RESUME));
                    break;
                case CLIENT_RDY:
                    data.push_back(std::byte(CLIENT_RDY));
                    break;
                case UID_ASSIGN:
                    data.push_back(std::byte(UID_ASSIGN));
                    data.push_back(std::byte(_shortUID));
                    break;
                default:
                    CUAssertLog(false, "Serializing invalid game state event type");
                }
                return data;
            }

            /**
             * This method unpacks all information from the byte vector
             * and stores it in this event.
             */
            void deserialize(const std::vector<std::byte>& data) override {
                Type flag = (Type)data[0];
                switch (flag) {
                case GAME_START:
                    _type = GAME_START;
                    break;
                case GAME_RESET:
                    _type = GAME_RESET;
                    break;
                case GAME_PAUSE:
                    _type = GAME_PAUSE;
                    break;
                case GAME_RESUME:
                    _type = GAME_RESUME;
                    break;
                case CLIENT_RDY:
                    _type = CLIENT_RDY;
                    break;
                case UID_ASSIGN:
                    _type = UID_ASSIGN;
                    _shortUID = (Uint8)data[1];
                    break;
                default:
                    CUAssertLog(false, "Deserializing game state event type");
                }
            }
        };

    }
}

#endif /* __CU_GAME_STATE_EVENT_H__ */