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
 * File: BusQueue.c
 * Date: October 5, 2022
 * Author(s) : JFowler
 * Editor(s) : JFowler
 *
 *******************************************************************************
 *
 * Description/Notes
 *
 *  Special queue for shared bus tasks built from the Queue and BusMutex
 *  modules. Each instance is only responsible for a single bus ID, hence,
 *  a bus queue should exist for every shared bus. Requires proper
 *  initialization and the dequeue routine to be called repeatedly.
 *
 ******************************************************************************/

#include "BusQueue.h"
#include "Utilities.h"

// *****************************************************************************

void BUSQUEUE_initialize(BUSQUEUE_instance_t* instance,
                         BUSMUTEX_instance_t* bus_mutex,
                         BUSMUTEX_bus_id_t bus_id,
                         uint8_t* queue_buffer,
                         uint32_t queue_buffer_length)
{
  UTILS_ASSERT(
    (NULL != instance) &&
    (NULL != bus_mutex) &&
    (NULL != queue_buffer) &&
    (queue_buffer_length >= sizeof(BUSQUEUE_element_t))
  );

  UTILITIES_memclear(instance, sizeof(BUSQUEUE_instance_t));

  instance->bus_mutex = bus_mutex;
  instance->bus_id = bus_id;

  QUEUE_initialize(&instance->queue,
                   queue_buffer,
                   queue_buffer_length,
                   sizeof(BUSQUEUE_element_t),
                   false,
                   false);
}

// *****************************************************************************

bool BUSQUEUE_enqueue(BUSQUEUE_instance_t* instance,
                      BUSQUEUE_task_callback_t task_callback,
                      uint32_t context,
                      bool discard_if_exists)
{
  UTILS_ASSERT(NULL != instance);
  bool element_enqueued = false;
  uint32_t scratch;

  BUSQUEUE_element_t element;
  element.task_callback = task_callback;
  element.context = context;

  if ((false == discard_if_exists) ||
      (false == QUEUE_get_element_position(&(instance->queue), (void*)&element, &scratch)))
  {
    element_enqueued = QUEUE_enqueue(&(instance->queue), &element);
  }

  return element_enqueued;
}

// *****************************************************************************

bool BUSQUEUE_dequeue(BUSQUEUE_instance_t* instance, bool force_dequeue)
{
  UTILS_ASSERT(NULL != instance);

  bool element_dequeued = false;
  BUSQUEUE_element_t element;

  // Is the bus available?

  if (true == BUSMUTEX_is_available(instance->bus_mutex, instance->bus_id))
  {
    // Peek at the next element, if it exists, attempt to start it.

    if (true == QUEUE_peek(&(instance->queue), (void*)&element))
    {
      // Dequeue the element if successfully started or if the force flag is
      // set.

      if ((true == element.task_callback(element.context)) ||
          (true == force_dequeue))
      {
        (void)QUEUE_dequeue(&(instance->queue), (void*)&element);
        element_dequeued = true;
      }
    }
  }

  return element_dequeued;
}
