SRCS=		luamotif.c widgets.c constants.c dialog.c wrapped_funcs.c callbacks.c application.c uibuilder.c utils.c widgetfactory.c WidgetClasses/GenericWidget.c WidgetClasses/SimpleMenuBar.c
LIB=		motif

LUA=		$(shell lua${SUFFIX} -v 2>&1 | cut -d ' ' -f 1)
LUAIMPLVER=	$(shell lua${SUFFIX} -v 2>&1 | cut -d ' ' -f 2 | cut -c 1-3)

ifeq (${LUA},LuaJIT)
  LUAVER=	5.1
  LUAINCVER=	-${LUAIMPLVER}
  LUALIBVER=	-${LUAVER}
else
  LUAVER=	${LUAIMPLVER}
  LUAINCVER=	${LUAVER}
  LUALIBVER=	${LUAVER}
endif

CFLAGS+=	-Wall -O3 -fPIC -I/usr/include -I${PKGDIR}/include \
		-I/usr/include/lua${SUFFIX}${LUAINCVER}
LDADD+=		-L${XDIR}/lib -L${PKGDIR}/lib -lXm -lXt -lX11 -lbsd -lgc

PKGDIR=		/usr
LIBDIR=		${PKGDIR}/lib
LUADIR=		${LIBDIR}/lua/${LUAVER}
${LIB}.so:	${SRCS:.c=.o}
		cc -shared -o ${LIB}.so ${CFLAGS} ${SRCS:.c=.o} ${LDADD}

clean:
		rm -f *.o *.so
		rm -f WidgetClasses/*.o
install:
	-mkdir -p ${DESTDIR}${LUADIR}
	install -m 755 ${LIB}.so ${DESTDIR}${LUADIR}/${LIB}.so
