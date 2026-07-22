#define __LIBTHECORE__
#include "stdafx.h"

#if defined(__FreeBSD__)

LPFDWATCH fdwatch_new(int nfiles)
{
    LPFDWATCH fdw;
    int kq;

    kq = kqueue();

    if (kq == -1)
    {
	sys_err("%s", strerror(errno));
	return NULL;
    }

    CREATE(fdw, FDWATCH, 1);

    fdw->kq = kq;
    fdw->nfiles = nfiles;
    fdw->nkqevents = 0;

    CREATE(fdw->kqevents, KEVENT, nfiles * 2);
    CREATE(fdw->kqrevents, KEVENT, nfiles * 2);
    CREATE(fdw->fd_event_idx, int, nfiles);
    CREATE(fdw->fd_rw, int, nfiles);
    CREATE(fdw->fd_data, void*, nfiles);

    return (fdw);
}

void fdwatch_delete(LPFDWATCH fdw)
{
    free(fdw->fd_data);
    free(fdw->fd_rw);
    free(fdw->kqevents);
    free(fdw->kqrevents);
    free(fdw->fd_event_idx);
    free(fdw);
}

int fdwatch(LPFDWATCH fdw, struct timeval *timeout)
{
    int	i, r;
    struct timespec ts;

    if (fdw->nkqevents)
	sys_log(2, "fdwatch: nkqevents %d", fdw->nkqevents);

    if (!timeout)
    {
	ts.tv_sec = 0;
	ts.tv_nsec = 0;

	r = kevent(fdw->kq, fdw->kqevents, fdw->nkqevents, fdw->kqrevents, fdw->nfiles, &ts);
    }
    else
    {
	ts.tv_sec = timeout->tv_sec;
	ts.tv_nsec = timeout->tv_usec;

	r = kevent(fdw->kq, fdw->kqevents, fdw->nkqevents, fdw->kqrevents, fdw->nfiles, &ts);
    }

    fdw->nkqevents = 0;

    if (r == -1)
	return -1;

    memset(fdw->fd_event_idx, 0, sizeof(int) * fdw->nfiles);

    for (i = 0; i < r; i++)
    {
	int fd = fdw->kqrevents[i].ident;

	if (fd >= fdw->nfiles)
	    sys_err("ident overflow %d nfiles: %d", fdw->kqrevents[i].ident, fdw->nfiles);
	else
	{
	    if (fdw->kqrevents[i].filter == EVFILT_WRITE)
		fdw->fd_event_idx[fd] = i;
	}
    }

    return (r);
}

void fdwatch_register(LPFDWATCH fdw, int flag, int fd, int rw)
{
    if (flag == EV_DELETE)
    {
	if (fdw->fd_rw[fd] & FDW_READ)
	{
	    fdw->kqevents[fdw->nkqevents].ident = fd;
	    fdw->kqevents[fdw->nkqevents].flags = flag;
	    fdw->kqevents[fdw->nkqevents].filter = EVFILT_READ;
	    ++fdw->nkqevents;
	}

	if (fdw->fd_rw[fd] & FDW_WRITE)
	{
	    fdw->kqevents[fdw->nkqevents].ident = fd;
	    fdw->kqevents[fdw->nkqevents].flags = flag;
	    fdw->kqevents[fdw->nkqevents].filter = EVFILT_WRITE;
	    ++fdw->nkqevents;
	}
    }
    else
    {
	fdw->kqevents[fdw->nkqevents].ident = fd;
	fdw->kqevents[fdw->nkqevents].flags = flag;
	fdw->kqevents[fdw->nkqevents].filter = (rw == FDW_READ) ? EVFILT_READ : EVFILT_WRITE; 

	++fdw->nkqevents;
    }
}

void fdwatch_clear_fd(LPFDWATCH fdw, socket_t fd)
{
    fdw->fd_data[fd] = NULL;
    fdw->fd_rw[fd] = 0;
}

void fdwatch_add_fd(LPFDWATCH fdw, socket_t fd, void * client_data, int rw, int oneshot)
{
	int flag;

	if (fd >= fdw->nfiles)
	{
		sys_err("fd overflow %d", fd);
		return;
	}

	if (fdw->fd_rw[fd] & rw)
		return;

	fdw->fd_rw[fd] |= rw;
	sys_log(2, "FDWATCH_REGISTER fdw %p fd %d rw %d data %p", fdw, fd, rw, client_data);

	if (!oneshot)
		flag = EV_ADD;
	else
	{
		sys_log(2, "ADD ONESHOT fd_rw %d", fdw->fd_rw[fd]);
		flag = EV_ADD | EV_ONESHOT;
		fdw->fd_rw[fd] |= FDW_WRITE_ONESHOT;
	}

	fdw->fd_data[fd] = client_data;
	fdwatch_register(fdw, flag, fd, rw);
}

void fdwatch_del_fd(LPFDWATCH fdw, socket_t fd)
{
    fdwatch_register(fdw, EV_DELETE, fd, 0);
    fdwatch_clear_fd(fdw, fd);
}

void fdwatch_clear_event(LPFDWATCH fdw, socket_t fd, unsigned int event_idx)
{
    assert(event_idx < fdw->nfiles * 2);

    if (fdw->kqrevents[event_idx].ident != fd)
	return;

    fdw->kqrevents[event_idx].ident = 0;
}

int fdwatch_check_event(LPFDWATCH fdw, socket_t fd, unsigned int event_idx)
{
    assert(event_idx < fdw->nfiles * 2);

    if (fdw->kqrevents[event_idx].ident != fd)
	return 0;

    if (fdw->kqrevents[event_idx].flags & EV_ERROR)
	return FDW_EOF;

    if (fdw->kqrevents[event_idx].flags & EV_EOF)
	return FDW_EOF;

    if (fdw->kqrevents[event_idx].filter == EVFILT_READ)
    {
	if (fdw->fd_rw[fd] & FDW_READ)
	    return FDW_READ;
    }
    else if (fdw->kqrevents[event_idx].filter == EVFILT_WRITE)
    {   
	if (fdw->fd_rw[fd] & FDW_WRITE)
	{ 
	    if (fdw->fd_rw[fd] & FDW_WRITE_ONESHOT)
		fdw->fd_rw[fd] &= ~FDW_WRITE;

	    return FDW_WRITE;
	}
    }
    else
	sys_err("fdwatch_check_event: Unknown filter %d (descriptor %d)", fdw->kqrevents[event_idx].filter, fd);

    return 0;
}

int fdwatch_get_ident(LPFDWATCH fdw, unsigned int event_idx)
{
    assert(event_idx < fdw->nfiles * 2);
    return fdw->kqrevents[event_idx].ident;
}

int fdwatch_get_buffer_size(LPFDWATCH fdw, socket_t fd)
{
    int event_idx = fdw->fd_event_idx[fd];

    if (fdw->kqrevents[event_idx].filter == EVFILT_WRITE)
	return fdw->kqrevents[event_idx].data;

    return 0;
}

void * fdwatch_get_client_data(LPFDWATCH fdw, unsigned int event_idx)
{
    int fd;

    assert(event_idx < fdw->nfiles * 2);

    fd = fdw->kqrevents[event_idx].ident;

    if (fd >= fdw->nfiles)
	return NULL;

    return (fdw->fd_data[fd]);
}

#elif defined(__LINUX__)

static uint32_t fdwatch_linux_events(int rw)
{
    uint32_t events = EPOLLERR | EPOLLHUP | EPOLLRDHUP;

    if (rw & FDW_READ)
	events |= EPOLLIN;
    if (rw & FDW_WRITE)
	events |= EPOLLOUT;

    return events;
}

static void fdwatch_linux_update(LPFDWATCH fdw, socket_t fd, int operation)
{
    struct epoll_event event;

    memset(&event, 0, sizeof(event));
    event.events = fdwatch_linux_events(fdw->fd_rw[fd]);
    event.data.fd = fd;

    if (epoll_ctl(fdw->epoll_fd, operation, fd, &event) == -1)
	sys_err("epoll_ctl(%d, %d): %s", operation, fd, strerror(errno));
}

LPFDWATCH fdwatch_new(int nfiles)
{
    LPFDWATCH fdw;
    int epoll_fd = epoll_create1(EPOLL_CLOEXEC);

    if (epoll_fd == -1)
    {
	sys_err("epoll_create1: %s", strerror(errno));
	return NULL;
    }

    CREATE(fdw, FDWATCH, 1);
    fdw->epoll_fd = epoll_fd;
    fdw->nfiles = nfiles;

    CREATE(fdw->epoll_events, struct epoll_event, nfiles);
    CREATE(fdw->events, FDWATCH_EVENT, nfiles * 2);
    CREATE(fdw->fd_rw, int, nfiles);
    CREATE(fdw->fd_data, void *, nfiles);

    return fdw;
}

void fdwatch_delete(LPFDWATCH fdw)
{
    close(fdw->epoll_fd);
    free(fdw->fd_data);
    free(fdw->fd_rw);
    free(fdw->events);
    free(fdw->epoll_events);
    free(fdw);
}

void fdwatch_clear_fd(LPFDWATCH fdw, socket_t fd)
{
    if (fd < 0 || fd >= fdw->nfiles)
	return;

    fdw->fd_data[fd] = NULL;
    fdw->fd_rw[fd] = 0;
}

void fdwatch_add_fd(LPFDWATCH fdw, socket_t fd, void * client_data, int rw, int oneshot)
{
    int old_rw;

    if (fd < 0 || fd >= fdw->nfiles)
    {
	sys_err("fd overflow %d", fd);
	return;
    }

    old_rw = fdw->fd_rw[fd];
    fdw->fd_rw[fd] |= rw;

    if (oneshot && (rw & FDW_WRITE))
	fdw->fd_rw[fd] |= FDW_WRITE_ONESHOT;

    fdw->fd_data[fd] = client_data;
    fdwatch_linux_update(fdw, fd, old_rw == FDW_NONE ? EPOLL_CTL_ADD : EPOLL_CTL_MOD);
}

void fdwatch_del_fd(LPFDWATCH fdw, socket_t fd)
{
    if (fd < 0 || fd >= fdw->nfiles || fdw->fd_rw[fd] == FDW_NONE)
	return;

    if (epoll_ctl(fdw->epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1 && errno != ENOENT && errno != EBADF)
	sys_err("epoll_ctl(DEL, %d): %s", fd, strerror(errno));

    fdwatch_clear_fd(fdw, fd);
}

int fdwatch(LPFDWATCH fdw, struct timeval * timeout)
{
    int timeout_ms = 0;
    int ready;
    int i;

    if (timeout)
    {
	long milliseconds = timeout->tv_sec * 1000L + timeout->tv_usec / 1000L;
	timeout_ms = milliseconds > INT_MAX ? INT_MAX : (int)milliseconds;
    }

    ready = epoll_wait(fdw->epoll_fd, fdw->epoll_events, fdw->nfiles, timeout_ms);
    if (ready == -1)
	return -1;

    fdw->event_count = 0;

    for (i = 0; i < ready; ++i)
    {
	struct epoll_event * event = &fdw->epoll_events[i];
	socket_t fd = event->data.fd;

	if (event->events & (EPOLLERR | EPOLLHUP))
	{
	    fdw->events[fdw->event_count].fd = fd;
	    fdw->events[fdw->event_count++].event = FDW_EOF;
	    continue;
	}

	if (event->events & (EPOLLIN | EPOLLRDHUP))
	{
	    fdw->events[fdw->event_count].fd = fd;
	    fdw->events[fdw->event_count++].event = FDW_READ;
	}

	if (event->events & EPOLLOUT)
	{
	    fdw->events[fdw->event_count].fd = fd;
	    fdw->events[fdw->event_count++].event = FDW_WRITE;
	}
    }

    return fdw->event_count;
}

int fdwatch_check_fd(LPFDWATCH fdw, socket_t fd)
{
    int result = FDW_NONE;
    int i;

    for (i = 0; i < fdw->event_count; ++i)
	if (fdw->events[i].fd == fd)
	    result |= fdw->events[i].event;

    return result;
}

void * fdwatch_get_client_data(LPFDWATCH fdw, unsigned int event_idx)
{
    socket_t fd;

    if (event_idx >= (unsigned int)fdw->event_count)
	return NULL;

    fd = fdw->events[event_idx].fd;
    if (fd < 0 || fd >= fdw->nfiles)
	return NULL;

    return fdw->fd_data[fd];
}

int fdwatch_get_ident(LPFDWATCH fdw, unsigned int event_idx)
{
    if (event_idx >= (unsigned int)fdw->event_count)
	return 0;

    return fdw->events[event_idx].fd;
}

void fdwatch_clear_event(LPFDWATCH fdw, socket_t fd, unsigned int event_idx)
{
    if (event_idx < (unsigned int)fdw->event_count && fdw->events[event_idx].fd == fd)
	fdw->events[event_idx].event = FDW_NONE;
}

int fdwatch_check_event(LPFDWATCH fdw, socket_t fd, unsigned int event_idx)
{
    int event;

    if (event_idx >= (unsigned int)fdw->event_count || fdw->events[event_idx].fd != fd)
	return FDW_NONE;

    event = fdw->events[event_idx].event;

    if (event == FDW_WRITE && (fdw->fd_rw[fd] & FDW_WRITE_ONESHOT))
    {
	fdw->fd_rw[fd] &= ~(FDW_WRITE | FDW_WRITE_ONESHOT);
	fdwatch_linux_update(fdw, fd, EPOLL_CTL_MOD);
    }

    return event;
}

int fdwatch_get_buffer_size(LPFDWATCH fdw, socket_t fd)
{
    (void)fdw;
    (void)fd;
    return INT_MAX;
}

#else

#ifdef __WIN32__
static int win32_init_refcount = 0;

static bool win32_init()
{
    if (win32_init_refcount > 0)
    {
	win32_init_refcount++;
	return true;
    }

    WORD wVersion = MAKEWORD(2, 0);
    WSADATA wsaData;

    if (WSAStartup(wVersion, &wsaData) != 0)
	return false;

    win32_init_refcount++;
    return true;
}

static void win32_deinit()
{
    if (--win32_init_refcount <= 0)
	WSACleanup();
}
#endif

LPFDWATCH fdwatch_new(int nfiles)
{
    LPFDWATCH fdw;

#ifdef __WIN32__
    if (!win32_init())
	return NULL;
#endif
	// nfiles value is limited to FD_SETSIZE (64)
    CREATE(fdw, FDWATCH, 1);
	fdw->nfiles = MIN(nfiles, FD_SETSIZE);

    FD_ZERO(&fdw->rfd_set);
    FD_ZERO(&fdw->wfd_set);

    CREATE(fdw->select_fds, socket_t, nfiles);
    CREATE(fdw->select_rfdidx, int, nfiles);

	fdw->nselect_fds = 0;

    CREATE(fdw->fd_rw, int, nfiles);
    CREATE(fdw->fd_data, void*, nfiles);

    return (fdw);
}

void fdwatch_delete(LPFDWATCH fdw)
{
    free(fdw->fd_data);
    free(fdw->fd_rw);
    free(fdw->select_fds);
    free(fdw->select_rfdidx);
    free(fdw);

#ifdef __WIN32__
    win32_deinit();
#endif
}

static int fdwatch_get_fdidx(LPFDWATCH fdw, socket_t fd) {
	int i;
	for (i = 0; i < fdw->nselect_fds; ++i) {
		if (fdw->select_fds[i] == fd) {
			return i;
		}
	}
	return -1;
}

void fdwatch_add_fd(LPFDWATCH fdw, socket_t fd, void* client_data, int rw, int oneshot)
{
	int idx = fdwatch_get_fdidx(fdw, fd);
	if (idx < 0) {
		if (fdw->nselect_fds >= fdw->nfiles) {
			return;
		}
		idx = fdw->nselect_fds;
		fdw->select_fds[fdw->nselect_fds++] = fd;
		fdw->fd_rw[idx] = rw;
	} else {
		fdw->fd_rw[idx] |= rw;
	}
	fdw->fd_data[idx] = client_data;

    if (rw & FDW_READ)
	FD_SET(fd, &fdw->rfd_set);

    if (rw & FDW_WRITE)
	FD_SET(fd, &fdw->wfd_set);
}

void fdwatch_del_fd(LPFDWATCH fdw, socket_t fd)
{
	if (fdw->nselect_fds <= 0) {
		return;
	}
    int idx = fdwatch_get_fdidx(fdw, fd);
	if (idx < 0) {
		return;
	}

	--fdw->nselect_fds;

	fdw->select_fds[idx] = fdw->select_fds[fdw->nselect_fds];
    fdw->fd_data[idx] = fdw->fd_data[fdw->nselect_fds];
    fdw->fd_rw[idx] = fdw->fd_rw[fdw->nselect_fds];

    FD_CLR(fd, &fdw->rfd_set);
    FD_CLR(fd, &fdw->wfd_set);
}

int fdwatch(LPFDWATCH fdw, struct timeval *timeout)
{
    int r, i, event_idx;
    struct timeval tv;

    fdw->working_rfd_set = fdw->rfd_set;
    fdw->working_wfd_set = fdw->wfd_set;

    if (!timeout)
    {
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	r = select(0, &fdw->working_rfd_set, &fdw->working_wfd_set, (fd_set*) 0, &tv);
    }
    else
    {
	tv = *timeout;
	r = select(0, &fdw->working_rfd_set, &fdw->working_wfd_set, (fd_set*) 0, &tv);
    }

    if (r == -1)
	return -1;

    event_idx = 0;

    for (i = 0; i < fdw->nselect_fds; ++i)
    {
		if (fdwatch_check_fd(fdw, fdw->select_fds[i]))
			fdw->select_rfdidx[event_idx++] = i;
    }

    return event_idx;
}

int fdwatch_check_fd(LPFDWATCH fdw, socket_t fd)
{
    int idx = fdwatch_get_fdidx(fdw, fd);
	if (idx < 0) {
		return 0;
	}
	int result = 0;
	if ((fdw->fd_rw[idx] & FDW_READ) && FD_ISSET(fd, &fdw->working_rfd_set)) {
		result |= FDW_READ;
	}
	if ((fdw->fd_rw[idx] & FDW_WRITE) && FD_ISSET(fd, &fdw->working_wfd_set)) {
		result |= FDW_WRITE;
	}
    return result;
}

void * fdwatch_get_client_data(LPFDWATCH fdw, unsigned int event_idx)
{
	int idx = fdw->select_rfdidx[event_idx];
	if (idx < 0 || fdw->nfiles <= idx) {
		return NULL;
	}
    return fdw->fd_data[idx];
}

int fdwatch_get_ident(LPFDWATCH fdw, unsigned int event_idx)
{
	int idx = fdw->select_rfdidx[event_idx];
	if (idx < 0 || fdw->nfiles <= idx) {
		return 0;
	}
	return (int)fdw->select_fds[idx];
}

void fdwatch_clear_event(LPFDWATCH fdw, socket_t fd, unsigned int event_idx)
{
	int idx = fdw->select_rfdidx[event_idx];
	if (idx < 0 || fdw->nfiles <= idx) {
		return;
	}
	socket_t rfd = fdw->select_fds[idx];
	if (fd != rfd) {
		return;
	}
    FD_CLR(fd, &fdw->working_rfd_set);
    FD_CLR(fd, &fdw->working_wfd_set);
}

int fdwatch_check_event(LPFDWATCH fdw, socket_t fd, unsigned int event_idx)
{
	int idx = fdw->select_rfdidx[event_idx];
	if (idx < 0 || fdw->nfiles <= idx) {
		return 0;
	}
	socket_t rfd = fdw->select_fds[idx];
	if (fd != rfd) {
		return 0;
	}
	int result = fdwatch_check_fd(fdw, fd);
	if (result & FDW_READ) {
		return FDW_READ;
	} else if (result & FDW_WRITE) {
		return FDW_WRITE;
	}
	return 0;
}

int fdwatch_get_buffer_size(LPFDWATCH fdw, socket_t fd)
{
    return INT_MAX; // XXX TODO
}

#endif
