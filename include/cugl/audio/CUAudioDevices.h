//
//  CUAudioDevices.h
//  Cornell University Game Library (CUGL)
//
//  This module is a singleton for managing audio devices in the game engine.
//  This singleton can support multiple input and output devices for complex
//  filter graphs. This class is for when developers needs direct access
//  to the audio devices.  In most cases, developers can use AudioEngine
//  instead, which is built on top of this device manager.
//
//  Because this is a singleton, there are no publicly accessible constructors
//  or intializers.  Use the static methods instead.
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
//  Author: Walker White
//  Version: 1/10/21
//
#ifndef __CU_AUDIO_DEVICES_H__
#define __CU_AUDIO_DEVICES_H__
#include <SDL.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

namespace cugl {

    /**
     * The audio graph classes.
     *
     * This internal namespace is for the audio graph clases.  It was chosen
     * to distinguish this graph from other graph class collections, such as the
     * scene graph collections in {@link scene2}.
     */
    namespace audio {
	    /** Forward references to some graph nodes */
        class AudioOutput;
        class AudioInput;
    }

/**
 * Class providing a singleton audio device manager
 *
 * This class is provides the most basic support for a modern audio engine.  It
 * has a factory for managing multiple input and output devices. However, it is
 * up to the developer to connect these together to form audio graphs.
 * Therefore, a developer should only use this class when direct access to the
 * audio graph is necessary.  Most developers can use {@link AudioEngine}
 * instead. As that class is built on top of this one, the developer should
 * only ever use one of the two classes.
 *
 * You cannot create new instances of this class.  Instead, you should access
 * the singleton through the three static methods: {@link start()}, {@link stop()},
 * and {@link get()}.
 *
 * IMPORTANT: Like the OpenGL context, this class is not thread-safe.  It is
 * only safe to access this class in the main application thread.  This means
 * it should never be called in a call-back function as those are typically
 * executed in the host thread.  If you need to access the AudioManager in a
 * callback function, you should use the {@link Application#schedule} method
 * to delay until the main thread is next available.
 */
class AudioDevices {
private:
    /** The singleton object for this class */
    static AudioDevices* _gManager;
    /** A mutex for synchronization purposes */
    std::mutex _mutex;
    /** Whether this AudioManager is currently active */
    bool _active;
    /** The output buffer size of this manager */
    Uint32 _output;
    /** The input buffer size of this manager */
    Uint32 _input;

    /** The list of all active output devices */
    std::unordered_map<std::string, std::shared_ptr<audio::AudioOutput>> _outputs;
    /** The list of all active input devices */
    std::unordered_map<std::string, std::shared_ptr<audio::AudioInput>>  _inputs;

#pragma mark -
#pragma mark Constructors (Private)
    /**
     * Creates, but does not initialize the singleton device manager
     *
     * The manager must be initialized before is can be used.
     */
    AudioDevices();

    /**
     * Disposes of the singleton device manager.
     *
     * This destructor releases all of the resources associated with this
     * audio manager.  Output and input devices can no longer be used, and no
     * instances of {@link AudioNode} may be created.
     */
    ~AudioDevices() { dispose(); }

    /**
     * Initializes the audio device manager.
     *
     * This method initializes the platform-specific audio manager, acquiring
     * any necessary resources.
     *
     * While input and output devices do not need to have uniform buffer sizes,
     * we require this to ensure that audio graph nodes are all interchangeable.
     *
     * @param output    The size of the read buffer for output devices
     * @param input     The size of the write buffer for input devices
     *
     * @return true if the audio manager was successfully initialized.
     */
    bool init(Uint32 output, Uint32 input);

    /**
     * Releases all resources for this singleton audio manager.
     *
     * Output and input devices can no longer be used, and no instances of
     * {@link AudioNode} may be created. If you need to use the manager again,
     * you must call init().
     */
    void dispose();


#pragma mark -
#pragma mark Static Attributes
public:
    /** The default read buffer size for each output device */
    static const Uint32 DEFAULT_READ_SIZE;

    /** The default write buffer size for each output device */
    static const Uint32 DEFAULT_WRITE_SIZE;

#pragma mark -
#pragma mark Static Accessors
    /**
     * Returns the singleton instance of the device manager.
     *
     * If the audio manager has not been started, then this method will return
     * nullptr.
     *
     * @return the singleton instance of the audio manager.
     */
    static AudioDevices* get() { return _gManager; }

    /**
     * Starts the singleton audio device manager.
     *
     * Once this method is called, the method get() will no longer return
     * nullptr.  Calling the method multiple times (without calling stop) will
     * have no effect. In addition, an audio manager will start off as inactive,
     * and must be activated.
     *
     * Instances of {@link audio::AudioNode} (and its subclasses) cannot be
     * initialized until this manager is activated.  That is because audio nodes
     * need a uniform buffer size (set by this method) in order to coordinate with
     * one another.
     *
     * This method will create a manager where the input and output buffer
     * sizes are the default values.
     */
    static void start();

    /**
     * Starts the singleton audio device manager.
     *
     * Once this method is called, the method get() will no longer return
     * nullptr.  Calling the method multiple times (without calling stop) will
     * have no effect. In addition, an audio manager will start off as inactive,
     * and must be activated.
     *
     * Instances of {@link audio::AudioNode} (and its subclasses) cannot be
     * initialized until this manager is activated.  That is because audio nodes
     * need a uniform buffer size (set by this method) in order to coordinate with
     * one another.
     *
     * This method will create a manager where the output and input buffer
     * share the same size.
     *
     * @param frames    The output and input buffer size in frames.
     */
    static void start(Uint32 frames);

    /**
     * Starts the singleton audio device manager.
     *
     * Once this method is called, the method get() will no longer return
     * nullptr.  Calling the method multiple times (without calling stop) will
     * have no effect. In addition, an audio manager will start off as inactive,
     * and must be activated.
     *
     * Instances of {@link audio::AudioNode} (and its subclasses) cannot be
     * initialized until this manager is activated.  That is because audio nodes
     * need a uniform buffer size (set by this method) in order to coordinate with
     * one another.
     *
     * This method will create a manager where the output and input buffer
     * have the specified sizes. It is not necessary for the buffer value of an
     * input device match the buffer value of an output device.  Indeed, on many
     * systems, an input buffer size of less than 1024 samples is not supported,
     * while output devices can process much faster than that. What is important
     * is ensuring enough delay so that the audio graph does not outrun the input
     * device. Therefore, an input delay of less than the input buffer size is
     * not recommended for real-time audio processing.
     *
     * @param output    The size of the output buffer in frames.
     * @param input     The size of the input buffer in frames.
     */
    static void start(Uint32 output, Uint32 input);

    /**
     * Stops the singleton device manager, releasing all resources.
     *
     * Once this method is called, the method get() will return nullptr.
     * Calling the method multiple times (without calling stop) will have
     * no effect.  In addition, the audio manager will no longer be active.
     *
     * Once this method is called, all instances of {@link audio::AudioNode} become
     * invalid.  In addition, no future instances of {@link audio::AudioNode} may
     * be created.  This method should only be called at application shutdown.
     */
    static void stop();

    /**
     * Returns the list of all the audio devices
     *
     * This value may change and should be polled regularly to provide an
     * up-to-date list.  The provided argument determines whether this is
     * for output or input devices.
     *
     * @param output    Whether to list output (instead of input) devices
     *
     * @return the list of all the audio devices
     */
    static std::vector<std::string> devices(bool output);

    /**
     * Returns the list of devices with attached audio nodes.
     *
     * If there is an audio node on the default device, this will include
     * the current default. The provided argument determines whether this is
     * for output or input devices.
     *
     * @param output    Whether to list output (instead of input) devices
     *
     * @return the list of devices with attached audio nodes.
     */
    static std::vector<std::string> occupied(bool output);

#pragma mark -
#pragma mark Manager Properties
    /**
     * Returns the size of the read buffer (in frames) for output nodes.
     *
     * While output devices do not need to have uniform buffer sizes, we
     * require this to ensure that audio graph nodes are all interchangeable.
     * Therefore, a suitable buffer size (that works for all relevant devices)
     * should be set at activation.
     *
     * Note that the value is in frames.  Therefore, output devices with
     * different numbers of channels will have a different raw buffer size.
     *
     * @return the size of the read buffer (in frames) for output nodes.
     */
    Uint32 getReadSize() const { return _output; }
    
    /**
     * Sets the size of the read buffer (in frames) for output nodes.
     *
     * While output devices do not need to have uniform buffer sizes, we
     * require this to ensure that audio graph nodes are all interchangeable.
     * Therefore, a suitable buffer size (that works for all relevant devices)
     * should be set at activation.
     *
     * Note that the value is in frames.  Therefore, output devices with
     * different numbers of channels will have a different raw buffer size.
     *
     * Changing this value has not effect on previously allocated output
     * devices. You should call {@link AudioEngine#setReadSize} instead.
     *
     * @param size  The size of the read buffer (in frames) for output nodes.
     */
    void setReadSize(Uint32 size);

    /**
     * Returns the size of the write buffer (in frames) for input nodes.
     *
     * While input devices do not need to have uniform buffer sizes, we
     * require this to ensure that audio graph nodes are all interchangeable.
     * Therefore, a suitable buffer size (that works for all relevant devices)
     * should be set at activation.
     *
     * Note that the value is in frames.  Therefore, input devices with
     * different numbers of channels will have a different raw buffer size.
     *
     * @return the size of the write buffer (in frames) for input nodes.
     */
    Uint32 getWriteSize() const { return _input; }
    
    /**
     * Sets the size of the write buffer (in frames) for input nodes.
     *
     * Unlike output devices, we do not chain device inputs. So the need for
     * a uniform write size is less urgent. However, we do often chain an
     * input node to an output node.  In that case, the write size defines
     * the delay (in frames) between when audio is written and when it
     * it is output to another device.
     *
     * Note that the value is in frames. Therefore, input devices with
     * different numbers of channels will have a different raw buffer size.
     *
     * Changing this value has not effect on previously allocated input
     * devices. You should call {@link audio#AudioInput#setWriteSize} instead.
     *
     * @param size  The size of the write buffer (in frames) for input nodes.
     */
    void setWriteSize(Uint32 size);

    /**
     * Returns true if the audio device manager is active.
     *
     * An active audio manager will regularly poll data from any unpaused
     * output node, and regular write data to any unreleased input node.
     *
     * @return true if the audio manager is active.
     */
    bool isActive();

    /**
     * Activates the audio device manager.
     *
     * This method is used to resume audio behavior after a call to the method
     * {@link deactivate()}.  This provides a uniform way of renabling audio
     * devices (such as after an application switch).
     *
     * This method is not the same as {@link start()}.  It does not allocate
     * any new resources.
     */
    void activate();

    /**
     * Deactivates the audio device manager.
     *
     * This method is used to pause all output nodes and release all input
     * nodes from recording.  This is important during an application switch,
     * such as when the game goes into the background.  All of the devices
     * may be resumed with a call to {@link deactivate()}.
     *
     * This method is not the same as {@link stop()}.  It does not release
     * any resources and no audio graphs are invalidated.
     */
    void deactivate();

    /**
     * Resets any stopped or failed audio devices.
     *
     * This method will also roll over the default output (not input) device
     * if it changes.
     *
     * This method is necessary for when an audio device is unplugged.  While
     * SDL often does this automatically, this method is provided for platforms
     * (e.g. CoreAudio on MacOS) where this must be done explicitly.
     */
    void reset();


#pragma mark -
#pragma mark Output Devices
    /**
     * Returns the default output device with 2 channels at 48000 Hz.
     *
     * An output device is initialized with both active and paused as false.
     * That means it will begin playback as soon as the audio manager is
     * activated.
     *
     * This node is always logically attached to the default output device.
     * That means it will switch devices whenever the default output changes.
     * This method may fail if the default device is in use.
     *
     * @return the default output device with 2 channels at 48000 Hz.
     */
    std::shared_ptr<audio::AudioOutput> openOutput();

    /**
     * Returns the default output device with the given channels and sample rate.
     *
     * An output device is initialized with both active and paused as false.
     * That means it will begin playback as soon as the audio manager is
     * activated.
     *
     * This node is always logically attached to the default output device.
     * That means it will switch devices whenever the default output changes.
     * This method may fail if the default output device is in use.
     *
     * @param channels  The number of audio channels
     * @param rate      The sample rate (frequency) in Hz
     *
     * @return the default output device with the given channels and sample rate.
     */
    std::shared_ptr<audio::AudioOutput> openOutput(Uint8 channels, Uint32 rate);

    /**
     * Returns the given output device with 2 channels at 48000 Hz.
     *
     * An output device is initialized with both active and paused as false.
     * That means it will begin playback as soon as the audio manager is
     * activated.
     *
     * This method may fail if the given device is in use.
     *
     * @param device    The name of the output device
     *
     * @return the given output device with 2 channels at 48000 Hz.
     */
    std::shared_ptr<audio::AudioOutput> openOutput(const char* device);

    /**
     * Returns the given output device with 2 channels at 48000 Hz.
     *
     * An output device is initialized with both active and paused as false.
     * That means it will begin playback as soon as the audio manager is
     * activated.
     *
     * This method may fail if the given device is in use.
     *
     * @param device    The name of the output device
     *
     * @return the given output device with 2 channels at 48000 Hz.
     */
    std::shared_ptr<audio::AudioOutput> openOutput(const std::string device);

    /**
     * Returns the output device with the given channels and sample rate.
     *
     * An output device is initialized with both active and paused as false.
     * That means it will begin playback as soon as the audio manager is
     * activated.
     *
     * This method may fail if the given device is in use.
     *
     * @param device    The name of the output device
     * @param channels  The number of audio channels
     * @param rate      The sample rate (frequency) in Hz
     *
     * @return the output device with the given channels and sample rate.
     */
    std::shared_ptr<audio::AudioOutput> openOutput(const char* device, Uint8 channels, Uint32 rate);

    /**
     * Returns the output device with the given channels and sample rate.
     *
     * An output device is initialized with both active and paused as false.
     * That means it will begin playback as soon as the audio manager is
     * activated.
     *
     * This method may fail if the given device is in use.
     *
     * @param device    The name of the output device
     * @param channels  The number of audio channels
     * @param rate      The sample rate (frequency) in Hz
     *
     * @return the output device with the given channels and sample rate.
     */
    std::shared_ptr<audio::AudioOutput> openOutput(const std::string device, Uint8 channels, Uint32 rate);

    /**
     * Closes the output device and disposes all resources.
     *
     * Once this method is called, the {@link audio::AudioOutput} is invalidated and
     * is no longer safe to use.
     *
     * @param device    The output device to close
     *
     * @return whether the device was successfully closed.
     */
    bool closeOutput(const std::shared_ptr<audio::AudioOutput>& device);

#pragma mark -
#pragma mark Input Devices
    /**
     * Returns the default input device with 2 channels at 48000 Hz.
     *
     * The input delay will be equal to the value {@link getWriteSize()}.
     * This means that playback is only available after two calls to
     * {@link audio::AudioInput#record()}.  This is the minimal value for smooth
     * real-time playback of recorded audio.
     *
     * An input device is initialized with both active as false and record as
     * true. That means it will start recording as soon as the AudioManager
     * is activated.  In addition, it is also unpaused, meaning that playback
     * will start as soon as it is attached to an audio graph.
     *
     * This node is always logically attached to the default input device.
     * That means it will switch devices whenever the default input changes.
     * This method may fail if the default device is in use.
     *
     * @return the default input device with 2 channels at 48000 Hz.
     */
    std::shared_ptr<audio::AudioInput> openInput();

    /**
     * Returns the default input device with the given channels and sample rate.
     *
     * The delay value is the number of frames that must be recorded before a
     * single frame.  This determines the playback latency.  While it is
     * possible to have a delay of 0, this is unlikely to provide smooth
     * real-time playback of recorded audio.  That is because there are no
     * guarantees about the thread interleaving of input and output devices.
     * A delay of at least {@link getWriteSize()}, and maybe even more, is
     * recommended.
     *
     * An input device is initialized with both active as false and record as
     * true. That means it will start recording as soon as the AudioManager
     * is activated.  In addition, it is also unpaused, meaning that playback
     * will start as soon as it is attached to an audio graph.
     *
     * This node is always logically attached to the default input device.
     * That means it will switch devices whenever the default input changes.
     * This method may fail if the default input device is in use.
     *
     * @param channels  The number of audio channels
     * @param rate      The sample rate (frequency) in Hz
     * @param delay     The playback delay between recording and reading
     *
     * @return the default input device with the given channels and sample rate.
     */
    std::shared_ptr<audio::AudioInput> openInput(Uint8 channels, Uint32 rate, Uint32 delay);

    /**
     * Returns the given input device with 2 channels at 48000 Hz.
     *
     * The input delay will be equal to the value {@link getWriteSize()}.
     * This means that playback is only available after two calls to
     * {@link audio::AudioInput#record()}.  This is the minimal value for smooth
     * real-time playback of recorded audio.
     *
     * An input device is initialized with both active as false and record as
     * true. That means it will start recording as soon as the AudioManager
     * is activated.  In addition, it is also unpaused, meaning that playback
     * will start as soon as it is attached to an audio graph.
     *
     * This method may fail if the given device is in use.
     *
     * @param device    The name of the output device
     *
     * @return the given input device with 2 channels at 48000 Hz.
     */
    std::shared_ptr<audio::AudioInput> openInput(const char* device);

    /**
     * Returns the given input device with 2 channels at 48000 Hz.
     *
     * The input delay will be equal to the value {@link getWriteSize()}.
     * This means that playback is only available after two calls to
     * {@link audio::AudioInput#record()}.  This is the minimal value for smooth
     * real-time playback of recorded audio.
     *
     * An input device is initialized with both active as false and record as
     * true. That means it will start recording as soon as the AudioManager
     * is activated.  In addition, it is also unpaused, meaning that playback
     * will start as soon as it is attached to an audio graph.
     *
     * This method may fail if the given device is in use.
     *
     * @param device    The name of the output device
     *
     * @return the given input device with 2 channels at 48000 Hz.
     */
    std::shared_ptr<audio::AudioInput> openInput(const std::string device);

    /**
     * Returns the given output device with the given channels and sample rate.
     *
     * The delay value is the number of frames that must be recorded before a
     * single frame.  This determines the playback latency.  While it is
     * possible to have a delay of 0, this is unlikely to provide smooth
     * real-time playback of recorded audio.  That is because there are no
     * guarantees about the thread interleaving of input and output devices.
     * A delay of at least {@link getWriteSize()}, and maybe even more, is
     * recommended.
     *
     * An input device is initialized with both active as false and record as
     * true. That means it will start recording as soon as the AudioManager
     * is activated. In addition, it is also unpaused, meaning that playback
     * will start as soon as it is attached to an audio graph.
     *
     * This method may fail if the given device is in use.
     *
     * @param device    The name of the output device
     * @param channels  The number of audio channels
     * @param rate      The sample rate (frequency) in Hz
     * @param delay     The playback delay between recording and reading
     *
     * @return the given output device with the given channels and sample rate.
     */
    std::shared_ptr<audio::AudioInput> openInput(const char* device, Uint8 channels, Uint32 rate, Uint32 delay);

    /**
     * Returns the given output device with the given channels and sample rate.
     *
     * The delay value is the number of frames that must be recorded before a
     * single frame.  This determines the playback latency.  While it is
     * possible to have a delay of 0, this is unlikely to provide smooth
     * real-time playback of recorded audio.  That is because there are no
     * guarantees about the thread interleaving of input and output devices.
     * A delay of at least {@link getWriteSize()}, and maybe even more, is
     * recommended.
     *
     * An input device is initialized with both active as false and record as
     * true. That means it will start recording as soon as the AudioManager
     * is activated.  In addition, it is also unpaused, meaning that playback
     * will start as soon as it is attached to an audio graph.
     *
     * This method may fail if the given device is in use.
     *
     * @param device    The name of the output device
     * @param channels  The number of audio channels
     * @param rate      The sample rate (frequency) in Hz
     * @param delay     The playback delay between recording and reading
     *
     * @return the given output device with the given channels and sample rate.
     */
    std::shared_ptr<audio::AudioInput> openInput(const std::string device, Uint8 channels, Uint32 rate, Uint32 delay);

    /**
     * Closes the output device and disposes all resources.
     *
     * Once this method is called, the {@link audio::AudioOutput} is invalidated and
     * is no longer safe to use.
     *
     * @param device    The output device to close
     *
     * @return whether the device was successfully closed.
     */
    bool closeInput(const std::shared_ptr<audio::AudioInput>& device);

};

}

#endif /* __CU_AUDIO_MANAGER_H__ */
