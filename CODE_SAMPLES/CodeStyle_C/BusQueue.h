/*******************************************************************************
 *         ,_     _
 *         |\\_,-~/
 *         / _  _ |    ,--.
 *        (  @  @ )   / ,-'
 *         \  _T_/-._( (
 *         /         `. \     __    ___   _____ __________
 *        |         _  \ |   / /   /   | / ___// ____/ __ \
 *         \ \ ,  /      |  / /   / /| | \__ \/ __/ / /_/ /
 *          || |-_\__   /  / /___/ ___ |___/ / /___/ _, _/
 *         ((_/`(____,-'  /_____/_/  |_/____/_____/_/ |_|
 *           ________  _____   _____ __________
 *          / ____/ / / /   | / ___// ____/ __ \
 *         / /   / /_/ / /| | \__ \/ __/ / /_/ /
 *        / /___/ __  / ___ |___/ / /___/ _, _/
 *        \____/_/ /_/_/  |_/____/_____/_/ |_|    (jwf)
 *
 *
 *  Authored property of Jared W. Fowler - jared.william.fowler@gmail.com
 *  All Rights Reserved. Unauthorized copying of this file, via any medium,
 *  is strictly prohibited. Proprietary and confidential.
 *
 *******************************************************************************
 *
 * File: BusQueue.h
 * Date: October 5, 2022
 * Author(s) : JFowler
 * Editor(s) : JFowler
 *
 *******************************************************************************
 *
 * Description/Notes
 *
 *  Special queue for shared bus tasks built from the Queue and BusMutex
 *  modules. Each instance is only responsible for a single bus ID, hence, a
 *  bus queue should exist for every shared bus. Requires proper initialization
 *  and the dequeue routine to be called repeatedly.
 *
 ******************************************************************************/

#ifndef BUS_QUEUE_JWF_H
#define BUS_QUEUE_JWF_H

#include "Queue.h"
#include "BusMutex.h"

// Support C++ builds.

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *
 * BUSQUEUE_task_callback_t
 *
 * DESCRIPTION:
 *  Function template for a user-provided function which is called to perform
 *  a new task on the instance shared bus.
 *
 * PARAMETERS:
 *  context
 *   User-provided context.
 *
 * RETURN:
 *  True if the task was started and can be removed from the queue, else,
 *  false.
 *
 ******************************************************************************/

typedef bool (*BUSQUEUE_task_callback_t)(uint32_t);

/*******************************************************************************
 *
 * BUSQUEUE_element_t
 *
 * DESCRIPTION:
 *  Data structure which is handled by the queue.
 *
 * context
 *  Value which will be passed into the user task callback function.
 *
 * task_callback
 *  User task function.
 *
 ******************************************************************************/

typedef struct
{
  uint32_t context;
  BUSQUEUE_task_callback_t task_callback;
}
BUSQUEUE_element_t;

/*******************************************************************************
 *
 * BUSQUEUE_flags_t
 *
 * DESCRIPTION:
 *  Module flags.
 *
 ******************************************************************************/

typedef union
{
  uint8_t all;
  struct
  {
    uint8_t reserved0                     : 8;
  };
}
BUSQUEUE_flags_t;

/*******************************************************************************
 *
 * BUSQUEUE_instance_t
 *
 * DESCRIPTION:
 *  Instance data and function pointers.
 *
 * flags
 *  Module flags.
 *
 * bus_mutex
 *  User-provided initialized instance of a BUSMUTEX.
 *
 * bus_id
 *  The bus ID of the shared bus associated with this instance.
 *
 * queue
 *  Queue instance contained and controlled by BusQueue. The user provides
 *  the buffer which is used by the queue.
 *
 ******************************************************************************/

typedef struct
{
  BUSQUEUE_flags_t flags;
  BUSMUTEX_instance_t* bus_mutex;
  BUSMUTEX_bus_id_t bus_id;
  QUEUE_instance_t queue;
}
BUSQUEUE_instance_t;

/*******************************************************************************
 *
 * BUSQUEUE_initialize
 *
 * DESCRIPTION:
 *  Initializes a module instance, erasing all data structures and setting
 *  default values.
 *
 * PARAMETERS:
 *  See BUSQUEUE_instance_t.
 *
 *  queue_buffer
 *   Pointer to user-provided buffer which will be used for the instance queue.
 *   The buffer size should be large enough to hold at least one or more
 *   BUSQUEUE_element_t.
 *
 *  queue_buffer_length
 *   Length, in bytes, of the queue buffer.
 *
 ******************************************************************************/

void BUSQUEUE_initialize(BUSQUEUE_instance_t* instance,
                         BUSMUTEX_instance_t* bus_mutex,
                         BUSMUTEX_bus_id_t bus_id,
                         uint8_t* queue_buffer,
                         uint32_t queue_buffer_length);

/*******************************************************************************
 *
 * BUSQUEUE_enqueue
 *
 * DESCRIPTION:
 *  Attempts to enqueue an new element.
 *
 * PARAMETERS:
 *  task_callback
 *   Task function to call when the element is dequeued.
 *
 *  context
 *   Value which will be passed into the user task callback function.
 *
 *  discard_if_exists
 *   If true, checks if the element already exists on the bus queue. If so,
 *   the new task element is discarded.
 *
 * RETURN
 *  True if space was available and the element was enqueued, else, false.
 *
 ******************************************************************************/

bool BUSQUEUE_enqueue(BUSQUEUE_instance_t* instance,
                      BUSQUEUE_task_callback_t task_callback,
                      uint32_t context,
                      bool discard_if_exists);

/*******************************************************************************
 *
 * BUSQUEUE_dequeue
 *
 * DESCRIPTION:
 *  Checks if the bus id, associated with the instance, is available. If so,
 *  peeks at the next element and calls its task function callback. If the
 *  callback returns true the element is dequeued.
 *
 * PARAMETERS:
 *  force_dequeue
 *   Will force the next element to dequeue even if the bus is busy and
 *   cannot start a new task. The dequeued task is simply deleted.
 *
 * RETURN
 *  True if the element was dequeued, else, false.
 *
 ******************************************************************************/

bool BUSQUEUE_dequeue(BUSQUEUE_instance_t* instance, bool force_dequeue);

#ifdef __cplusplus
}
#endif
#endif // BUS_MUTEX_JWF_H
