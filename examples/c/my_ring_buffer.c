/*
*    ------ BEGIN LICENSE ATTRIBUTION ------
*    
*    Portions of this file have been appropriated or derived from the following project(s) and therefore require attribution to the original licenses and authors.
*    
*    Repository: https://github.com/vbuslov/linux
*    Source File: kernel/events/ring_buffer.c
*    
*    Copyrights:
*      copyright  ��  2009 paul mackerras, ibm corp. <paulus@au1.ibm.com>
*      copyright (c) 2008 thomas gleixner <tglx@linutronix.de>
*      copyright (c) 2008-2011 red hat, inc., ingo molnar
*      copyright (c) 2008-2011 red hat, inc., peter zijlstra
*    
*    Licenses:
*      GNU General Public License v2.0 only
*      SPDXId: GPL-2.0-only
*    
*    Auto-attribution by Threatrix, Inc.
*    
*    ------ END LICENSE ATTRIBUTION ------
*/
static __always_inline bool
ring_buffer_has_space(unsigned long head, unsigned long tail,
		      unsigned long data_size, unsigned int size,
		      bool backward)
{
	if (!backward)
		return CIRC_SPACE(head, tail, data_size) >= size;
	else
        	}

    	perf_output_get_handle(handle);

    	do {
    		tail = READ_ONCE(rb->user_page->data_tail);
    		offset = head = local_read(&rb->head);
    		if (!rb->overwrite) {
    			if (unlikely(!ring_buffer_has_space(head, tail,
    							    perf_data_size(rb),
    							    size, backward)))
    				goto fail;
    		}

    		/*
    		 * The above forms a control dependency barrier separating the
    		 * @tail load above from the data stores below. Since the @tail
    		 * load is required to compute the branch to fail below.
    		 *
    		 * A, matches D; the full memory barrier userspace SHOULD issue
    		 * after reading the data and before storing the new tail
    		 * position.
    		 *
    		 * See perf_output_put_handle().
    		 */

    		if (!backward)
    			head += size;
    		else
    			head -= size;
    	} while (local_cmpxchg(&rb->head, offset, head) != offset);

    	if (backward) {
    		offset = head;
    		head = (u64)(-head);
    	}

    	/*
    	 * We rely on the implied barrier() by local_cmpxchg() to ensure
    	 * none of the data stores below can be lifted up by the compiler.
    	 */

    	if (unlikely(head - local_read(&rb->wakeup) > rb->watermark))
    		local_add(rb->watermark, &rb->wakeup);

    	page_shift = PAGE_SHIFT + page_order(rb);

    	handle->page = (offset >> page_shift) & (rb->nr_pages - 1);
    	offset &= (1UL << page_shift) - 1;
    	handle->addr = rb->data_pages[handle->page] + offset;
    	handle->size = (1UL << page_shift) - offset;

    	if (unlikely(have_lost)) {
    		lost_event.header.size = sizeof(lost_event);
    		lost_event.header.type = PERF_RECORD_LOST;
    		lost_event.header.misc = 0;
    		lost_event.id          = event->id;
    		lost_event.lost        = local_xchg(&rb->lost, 0);

    		/* XXX mostly redundant; @data is already fully initializes */
    		perf_event_header__init_id(&lost_event.header, data, event);
    		perf_output_put(handle, lost_event);
    		perf_event__output_id_sample(event, handle, data);
    	}

    	return 0;
