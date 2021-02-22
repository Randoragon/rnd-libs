/** @page rnd-game-overview The Game Library Overview
 *
 * Here's a brief explainer on some features of the library, the inner-workings
 * of some structs and functions, and most importantly, the general thought
 * behind each struct, the intended workflow, etc. This page is @b not a complete
 * documentation (for more you can refer to the @ref RND_Game.h file documentation),
 * I just spill some of my thoughts in here to keep me on track
 * with what I want to do with the library.
 * 
 * @b NOTE: Some of the concepts explained here may be outdated or not available in the
 * library yet. Again, I'm writing this file mainly to organize everything in my head.
 * 
 * @section sec0 Quick Rundown (tl;dr)
 * 
 * - Objects are immutable templates for creating mutable instances.
 * - Event handlers are arrays of function pointers, accessed by object indices.
 * 
 * If you don't understand what this means, that's okay, everything from this point
 * onward is going to be explained in great depth. Don't forget that you can always
 * refer to the <a href=https://github.com/randoragon/rnd-libs/tree/master/game>source code</a>
 * for more details!
 * 
 * @section sec1 1. Objects
 * 
 * @subsection sec1-1 1.1 Overview
 * 
 * An "object", theoretically speaking, is a blueprint or schematic for something.
 * That "something" can be a video game entity (a character, enemy, wall, etc.),
 * or something completely different, like a particle, or just some invisible to
 * the player piece of code running calculations in the background.
 * 
 * Objects are identified by an ID number (@ref RND_GameObjectIndex).
 * Objects do not themselves get "spawned" in the game, they act as a template
 * for creating an @e instance. No instance can exist without a corresponding
 * object, although objects can exist without corresponding instances. Moreover,
 * multiple instances of the same object can coexist at the same time.
 * 
 * @subsection sec1-2 1.2 Creating an Object
 * 
 * Internally, an object is a struct of variables which get passed onto every
 * instance of that object. Creating a new object involves writing a new struct
 * and populating it with member variables that an instance will need. For example:
 * 
 * @code
 * typedef struct
 * {
 *     int x, y;
 *     float health;
 *     char *name;
 * } ObjectPlayer;
 * @endcode
 * 
 * It is up to the user to pick an appropriate object index and keep track of it.
 * The recommended way of doing this is with a separate header file for macros:
 * 
 * @code
 * #define OBJECT_INDEX_PLAYER 0
 * #define OBJECT_INDEX_WALL   1
 * #define OBJECT_INDEX_ENEMY  2
 * ...
 * @endcode
 * 
 * Once you have these two things, you can "add" the object and the library will
 * remember it:
 * 
 * @code
 * RND_gameObjectAdd("ObjectPlayer", OBJECT_INDEX_PLAYER, sizeof(ObjectPlayer));
 * @endcode
 * 
 * The first argument is a string representing the object. This can be anything you
 * want, but it cannot be empty. This string is used for example in warning/error
 * messages to make it easier to identify objects, because for humans it's faster
 * to recognize than an index number.
 * 
 * Typically you want the name string to be the same as the name of the object struct.
 * Because it's a bit painful to have to write the same thing twice, there's a macro
 * that automatically calls @ref RND_gameObjectAdd with the right parameters:
 * 
 * @code
 * #define RND_GAME_OBJECT_ADD(struct, index) RND_gameObjectAdd(#struct, index, sizeof(struct))
 * @endcode
 * 
 * Hence, this is the @b recommended way of adding new objects:
 * 
 * @code
 * RND_GAME_OBJECT_ADD(ObjectPlayer, OBJECT_INDEX_PLAYER);
 * @endcode
 * 
 * You can obtain an object's name string with @c RND_gameObjectGetName(index).
 * 
 * @e Optionally, you can write an object constructor and destructor. Both of them
 * need to be functions that intake a void pointer (to the object struct) and
 * return an error code (0 for success):
 * 
 * @code
 * int objectPlayerCtor(void *object)
 * {
 *     ObjectPlayer *o = (ObjectPlayer*)object;
 *     o->x = 0; o->y = 0; o->health = 15.0;
 *     if (!(o->name = (char*)malloc(sizeof(char) * 35)))
 *         return 1;
 *     return 0;
 * }
 * 
 * int objectPlayerDtor(void *object) {
 *     free(((ObjectPlayer*)object)->name);
 *     return 0;
 * }
 * @endcode
 * 
 * Once they're written, all you need to do is assign them to the @ref RND_ctors
 * and @ref RND_dtors arrays. They are both built-in and globally included by
 * the library:
 * 
 * @code
 * RND_ctors[OBJECT_INDEX_PLAYER] = objectPlayerCtor;
 * RND_dtors[OBJECT_INDEX_PLAYER] = objectPlayerDtor;
 * @endcode
 * 
 * If either the constructor or destructor returns non-0, an adequate warning
 * message will be printed to @c stderr during instance initialization/destruction.
 * 
 * 
 * @section sec2 2. Instances
 * 
 * @subsection sec2-1 2.1 Overview
 * 
 * An instance is an independent copy of an object's struct data. For example,
 * a typical game will have a player and an enemy object, but there's only
 * one @e instance of a player object, while there may be multiple instances
 * of enemy objects. Instances are containers for meaningful information about
 * what's happening in the game. That information can be altered during runtime
 * as you see fit.
 * 
 * @subsection sec2-2 2.2 Spawning an Instance
 * 
 * Once you've created an object, you can create an instance of it like so:
 * 
 * @code
 * RND_GameInstanceId id = RND_gameInstanceSpawn(OBJECT_INDEX_PLAYER);
 * @endcode
 * 
 * @ref RND_gameInstanceSpawn will return a unique @b instance id which can be used
 * in various instance functions. Pretty much all functions that operate on
 * instances use the instance id as an identifier.
 * 
 * An instance id is of @c uintmax_t type, and every single instance must
 * have a unique one during a single runtime, so theoretically if you ran out of
 * numbers (0 is reserved for errors and exceptions), the library would
 * crash with a fatal error.  To give some perspective though, pretty much all
 * modern computers use 64-bit processors, and to grind through 2^64 ids you would
 * need to spawn 1000000 instances per tick in a 60FPS game over the course of
 * nearly 10 thousand years, so you would most definitely run out of memory first
 * anyway.
 * 
 * @subsection sec2-3 2.3 The Instances Array
 * 
 * All spawned instances that have not yet been killed reside in the global
 * @ref RND_instances array. The array is composed of @ref RND_GameInstance structs:
 * 
 * @code
 * struct RND_GameInstance
 * {
 *     bool is_alive;
 *     RND_GameObjectIndex index;
 *     void *data;
 * };
 * @endcode
 * 
 * The array is indexed by instance ids with @c id=0 reserved as "no instance".
 * Instance ids are 64-bit integers and it's impossible to allocate that big
 * of an array, so the starting size is 2^16 elements, and if the instance
 * count during runtime crosses that threshold, the array size is doubled.
 * The array's current size is stored in @ref RND_instances_size.
 * It is therefore possible to iterate through all existing instances and
 * access their object structs by doing the following:
 * 
 * @code
 * for (RND_GameInstanceId id = 1; id < RND_instances_size; id++) {
 *     RND_GameInstance *inst = RND_instances + id;
 *     // For example, to set all players' health to 0:
 *     if (inst->is_alive && inst->index == OBJECT_INDEX_PLAYER) {
 *         ((ObjectPlayer*)inst->data)->health = 0;
 *     }
 * }
 * @endcode
 * 
 * @subsection sec2-4 2.4 Accessing an Instance
 * 
 * In order to access instance variables (variables from the object struct),
 * you need its unique instance id and the corresponding object type:
 * 
 * @code
 * ObjectPlayer *data = (ObjectPlayer*)(RND_instances[id].data);
 * data->x = 15;
 * @endcode
 * 
 * Since it is very common to operate on instance variables, and a little
 * encumbering to have to recast void pointers every single time, there's
 * a macro that slightly simplifies this process:
 * 
 * @code
 * #define RND_GAME_INST(id, struct) (*((struct*)(RND_instances[id].data))) 
 * @endcode
 * 
 * Here's the equivalent of the first operation on @c ObjectPlayer, this time
 * using the macro shortcut:
 * 
 * @code
 * RND_GAME_INST(id, ObjectPlayer).x = 15;
 * @endcode
 * 
 * @subsection sec2-5 2.5 Killing an Instance
 * 
 * Once an instance's job is finished, you should deallocate its memory and
 * set its @c is_alive member variable to @c false. This can be accomplished
 * by using the @ref RND_gameInstanceKill function:
 * 
 * @code
 * RND_gameInstanceKill(id);
 * @endcode
 * 
 * If no instance with the supplied id is alive, the function will print
 * a warning message and peacefully exit, returning 0 for success.
 * 
 * @section sec3 3. Event Handlers
 * 
 * @subsection sec3-1 3.1 Overview
 * 
 * The majority of game engines operate on events which execute specific code.
 * For example, for each object type there's separate code that gets executed
 * every step, but there also might be code that gets executed on button events,
 * or some custom events orchestrated by the developer that run scripts.
 * 
 * In short, the right way to handle events for all spawned instances is to
 * create a new event handler. If execution order is important, then you will need
 * to write a @b priority @b function, which is a function that intakes an instance
 * and returns an integer denoting that instance's priority. For example, in the
 * context of handling a draw event, instances with lower priority values will
 * be drawn earlier than instances with higher priority, which will result in the
 * latter being drawn on top of the former.
 * 
 * @subsection sec3-2 3.2 Example Usage
 * 
 * Here's an example of how to create a button event handler
 * for our @c ObjectPlayer which will cut its health in half.
 * 
 * First, we need to decide whether or not we need a priority function. For this
 * example we will skip this step, because it is not needed when we're considering
 * a button even with only one receiving object type. However, to illustrate what
 * priority functions may look like, here's an arbitrary example:
 * 
 * @code
 * int examplePriorityFunc(const RND_GameInstance *inst)
 * {
 *     switch(inst->index) {
 *         case OBJECT_INDEX_PLAYER: return 2;
 *         case OBJECT_INDEX_ENEMY:  return -2;
 *         case OBJECT_INDEX_WALL:   return 1;
 *         default:                  return 0;
 *     }
 * }
 * @endcode
 * 
 * A couple things to note about priority functions:
 * @li It doesn't matter what method you use to determine the return value. In
 * the above example I used @ref RND_GameInstance::index, but you can do anything
 * you want, like accessing specific object types' instance data or even randomness.
 * @li Priority functions absolutely should not modify instances!
 * @li The return values by themselves don't mean anything, it's just the relations
 * between them that influence the call order.
 * 
 * Step 2, write a handler function for each object that needs it (in our case
 * it's only the player, but normally you would have multiple functions, one
 * for each object type that's relevant):
 * 
 * @code
 * int objectPlayerEventButtonHandler(RND_GameInstance *self)
 * {
 *     // it's convenient to make a pointer to raw instance data
 *     ObjectPlayer *o = (ObjectPlayer*)self->data;
 *     o->health /= 2.0;
 *     return 0;
 * }
 * @endcode
 * 
 * Then, initialize a new handlers array and add the handler to it:
 * 
 * @code
 * RND_GameHandler *button_handlers = RND_gameHandlerCreate(NULL);
 * if (button_handlers) {
 *     // Assign the right handler function to an object index
 *     RND_gameHandlerAdd(button_handlers, OBJECT_INDEX_PLAYER, objectPlayerEventButtonHandler);
 * } else {
 *     fprintf(stderr, "failed to allocate heap memory\n");
 * }
 * @endcode
 *
 * @ref RND_gameHandlerCreate intakes a priority function pointer,
 * but you can pass it @c NULL and everything will be treated equally
 * like in an ordinary FIFO queue (all priorities will default to 0).
 * 
 * That's everything as far as setting up goes. From this point onward, every single
 * instance that gets spawned or dies will be added or removed from the handler's
 * priority queue. This makes executing the handler code as simple as iterating through
 * the priority queue and calling each instance's respective handler function, which is
 * precisely what the @ref RND_gameHandlerRun function does:
 * 
 * @code
 * if (button_event_received) {
 *     RND_gameHandlersRun(button_handlers);
 * }
 * @endcode
 * 
 * That's all there is to it. Please note that event handlers have nothing to do with event
 * detection, they merely @b handle them. In the above code snippet, it is up to some external
 * library to detect when the button in question is actually pressed (@c button_event_received).
 * 
 */
