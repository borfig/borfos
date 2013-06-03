import sys, os

def main(input_filename):
    l = {}
    exec open(input_filename, 'rb') in {}, l
    deps = l['DEPENDENCIES']
    deps = dict((t, set(d)) for t, d in deps.iteritems())
    result = []
    while deps:
        ready = [t for (t, d) in deps.iteritems() if not d]
        if not ready:
            raise Exception('circular dependency found')
        for r in ready:
            result.append(' \\\n    CONSTRUCTOR_ITEM(%s)' % (r,))
            del deps[r]
        for t, d in deps.iteritems():
            for r in ready:
                d.discard(r)
    protector = os.path.basename(input_filename).replace('.py', '_h').upper()
    print '#ifndef %s' % (protector)
    print '#define %s' % (protector)
    print
    print '#define CONSTRUCTORS',
    for r in result:
        print r,
    print
    print '#endif /* %s */' % (protector)

if __name__ == '__main__':
    main(*sys.argv[1:])
