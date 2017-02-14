#!/usr/bin/env python
"""Doxygen XML to SWIG docstring converter.

Usage:

  doxy2swig.py [options] input.xml output.i

Converts Doxygen generated XML files into a file containing docstrings
that can be used by SWIG-1.3.x.  Note that you need to get SWIG
version > 1.3.23 or use Robin Dunn's docstring patch to be able to use
the resulting output.

input.xml is your doxygen generated XML file and output.i is where the
output will be written (the file will be clobbered).

"""
######################################################################
#
# This code is implemented using Mark Pilgrim's code as a guideline:
#   http://www.faqs.org/docs/diveintopython/kgp_divein.html
#
# Author: Prabhu Ramachandran
# License: BSD style
#
# Thanks:
#   Johan Hake:  the include_function_definition feature
#   Bill Spotz:  bug reports and testing.
#   Sebastian Henschel:   Misc. enhancements.
#
######################################################################

#_*_ coding:utf-8 _*_
from xml.dom import minidom
import re
import textwrap
import sys
import types
import os.path
import optparse
import os

def my_open_read(source):
    if hasattr(source, "read"):
        return source
    else:
        return open(source)

def my_open_write(dest):
    if hasattr(dest, "write"):
        return dest
    else:
        return open(dest, 'w')


class Doxy2SWIG:
    """Converts Doxygen generated XML files into a file containing
    docstrings that can be used by SWIG-1.3.x that have support for
    feature("docstring").  Once the data is parsed it is stored in
    self.pieces.

    """

    def __init__(self, src, include_function_definition=True, quiet=False):
        """Initialize the instance given a source object.  `src` can
        be a file or filename.  If you do not want to include function
        definitions from doxygen then set
        `include_function_definition` to `False`.  This is handy since
        this allows you to use the swig generated function definition
        using %feature("autodoc", [0,1]).

        """
        f = my_open_read(src)
        self.my_dir = os.path.dirname(f.name)
        self.xmldoc = minidom.parse(f).documentElement
        f.close()

        self.pieces = []
        self.pieces.append('\n// File: %s\n'%\
                           os.path.basename(f.name))

        self.space_re = re.compile(r'\s+')
        self.lead_spc = re.compile(r'^(%feature\S+\s+\S+\s*?)"\s+(\S)')
        self.multi = 0
        self.ignores = ['inheritancegraph', 'param', 'listofallmembers',
                        'innerclass', 'name', 'declname', 'incdepgraph',
                        'invincdepgraph', 'type',
                        'references', 'referencedby', 'location',
                        'collaborationgraph', 'reimplements',
                        'reimplementedby', 'derivedcompoundref',
                        'basecompoundref']
        #self.generics = []
        self.include_function_definition = include_function_definition
        if not include_function_definition:
            self.ignores.append('argsstring')

        self.quiet = quiet
        self.list_ctr = 1  #counts the number of spaces to be displayed before displaying a list item
        self.simplesect_kind = ''
        self.para_kind = ''

    def generate(self):
        """Parses the file set in the initialization.  The resulting
        data is stored in `self.pieces`.

        """
        self.parse(self.xmldoc)

    def parse(self, node):
        """Parse a given node.  This function in turn calls the
        `parse_<nodeType>` functions which handle the respective
        nodes.

        """
        pm = getattr(self, "parse_%s"%node.__class__.__name__)
        pm(node)

    def parse_Document(self, node):
        #print("himanshu ::::::: parse Document... ")
        self.parse(node.documentElement)

    def parse_Text(self, node):
        #print("himanshu ::::::: parse Text... ")
        txt = node.data
        #txt = txt.replace('\\', r'\\\\')
        txt = txt.replace('"', r'\"')
        #print '--------------------------------------'
        #print '--------------------------------------'
        #print txt
        # ignore pure whitespace
        m = self.space_re.match(txt)
        if m and len(m.group()) == len(txt):
            pass
        else:
            #self.add_text(txt)
            t = textwrap.fill(txt, 100, break_long_words=False)
            #print 'HIMANSHU ---------- >>>>>>>>>>>>>>>>>>>>> '
            #print t
            t = t.replace('\n','\n* '+' '*(self.list_ctr+2))
            #t = t.replace('1234',' '*self.list_ctr)
            if t:
                self.add_line = 1
            self.add_text(t)

    def parse_Element(self, node):
        """Parse an `ELEMENT_NODE`.  This calls specific
        `do_<tagName>` handers for different elements.  If no handler
        is available the `generic_parse` method is called.  All
        tagNames specified in `self.ignores` are simply ignored.

        """
        #print("himanshu ::::::: parse Element... ")
        name = node.tagName
        ignores = self.ignores
        if name in ignores:
            return
        attr = "do_%s" % name
        if hasattr(self, attr):
            handlerMethod = getattr(self, attr)
            handlerMethod(node)
        else:
            self.generic_parse(node)
            #if name not in self.generics: self.generics.append(name)

    def parse_Comment(self, node):
        """Parse a `COMMENT_NODE`.  This does nothing for now."""
        return

    def add_text(self, value):
        """Adds text corresponding to `value` into `self.pieces`."""
        #print value
        if type(value) in (types.ListType, types.TupleType):
            self.pieces.extend(value)
        else:
            self.pieces.append(value)

    def get_specific_nodes(self, node, names):
        """Given a node and a sequence of strings in `names`, return a
        dictionary containing the names as keys and child
        `ELEMENT_NODEs`, that have a `tagName` equal to the name.

        """
        nodes = [(x.tagName, x) for x in node.childNodes \
                 if x.nodeType == x.ELEMENT_NODE and \
                 x.tagName in names]
        return dict(nodes)

    def generic_parse(self, node, pad=0):
        """A Generic parser for arbitrary tags in a node.

        Parameters:

         - node:  A node in the DOM.
         - pad: `int` (default: 0)

           If 0 the node data is not padded with newlines.  If 1 it
           appends a newline after parsing the childNodes.  If 2 it
           pads before and after the nodes are processed.  Defaults to
           0.

        """
        npiece = 0
        if pad:
            npiece = len(self.pieces)
        if pad == 2:
            self.add_text('\n* ')
        for n in node.childNodes:
            self.parse(n)
        if pad:
            #if len(self.pieces) > npiece:
            self.add_text('\n* ')

    def space_parse(self, node):
        self.add_text(' ')
        self.generic_parse(node)

    def do_compoundname(self, node):
        self.add_text('\n\n')
        data = node.firstChild.data
        #self.add_text('%feature("docstring") %s "\n'%data)
        self.add_text('%typemap(csclassmodifiers) ')
        self.add_text('%s\n"\n/**\n'%data)

    def do_compounddef(self, node):
        kind = node.attributes['kind'].value
        if kind in ('class', 'struct'):
            prot = node.attributes['prot'].value
            if prot <> 'public':
                return
            names = ('compoundname', 'briefdescription',
                     'detaileddescription', 'includes')
            first = self.get_specific_nodes(node, names)
            for n in names:
                if first.has_key(n):
                    self.parse(first[n])
            #self.add_text(['";','\n'])
            self.add_text(['*/','\n','%s %s ";'%(prot,'class'),'\n'])
            for n in node.childNodes:
                if n not in first.values():
                    self.parse(n)
        elif kind in ('file', 'namespace'):
            nodes = node.getElementsByTagName('sectiondef')
            for n in nodes:
                self.parse(n)

    def do_includes(self, node):
        self.add_text('\n* @include ')
        self.generic_parse(node, pad=1)

    def do_parameterlist(self, node):

        #print("himanshu ::::::::::  do_parameterlist")
        text='unknown'
        for key, val in node.attributes.items():
            """if key == 'kind':
                if val == 'param': text = 'Parameters'
                elif val == 'exception': text = 'Exceptions'
                else: text = val
                break"""
            if key == 'kind':
                if val == 'param': text = '@param'
                elif val == 'exception': text = '@exception'
                else: text = val
                break
        #self.add_text(['\n', '\n', text, ':', '\n'])
        #self.add_text(['\n', '* ', text])
        self.para_kind = text
        self.generic_parse(node, pad=0)

    def do_para(self, node):
        #print("himanshu :::::::: do_para ")
        #self.add_text(['\n'])
        self.generic_parse(node, pad=0)

    def do_parametername(self, node):
        #print("himanshu :::::::: do_parametername")
        self.add_text(['\n', '* ', self.para_kind])

        try:
            data=node.firstChild.data
        except AttributeError: # perhaps a <ref> tag in it
            data=node.firstChild.firstChild.data
        if data.find('Exception') != -1:
            #print("himanshu :::::::: Pronting DAta1")
            #print data
            self.add_text(data)
        else:
            #print("himanshu :::::::: Pronting DAta2")
            #print data
            for key, val in node.attributes.items():
                if key == 'direction':
                    self.add_text('[%s] '%val)
            self.add_text("%s "%data)

    def do_parameterdefinition(self, node):
        self.generic_parse(node, pad=1)

    def do_detaileddescription(self, node):
        #self.add_text('')
        self.generic_parse(node, pad=0)

    def do_briefdescription(self, node):
        self.add_text("* @brief ")
        self.generic_parse(node, pad=1)

    def do_memberdef(self, node):
        prot = node.attributes['prot'].value
        id = node.attributes['id'].value
        kind = node.attributes['kind'].value
        tmp = node.parentNode.parentNode.parentNode
        compdef = tmp.getElementsByTagName('compounddef')[0]
        cdef_kind = compdef.attributes['kind'].value
        #print('Himanshu :: ...... Memberdef........')
        #print('prot= %s ....., id= %s ....., kind= %s..... ,cdef_kind= %s'%(prot,id,kind,cdef_kind))

        if prot == 'public':
            #print('Entering here')
            first = self.get_specific_nodes(node, ('definition', 'name'))
            #print first
            name = first['name'].firstChild.data
            #print name
            if name[:8] == 'operator': # Don't handle operators yet.
                return
            #print('Entering here2')

            # For ENUMS
            """if kind == 'enum':
                #print('himanshu is in enum now')
                self.add_text('\n\n')
                self.add_text('%typemap(csclassmodifiers) ')
                self.add_text('%s\n"\n/**\n'%data)
                self.generic_parse(node, pad=0)
            """
            ##################################################
            # For Classes & Functions
            if not first.has_key('definition') or \
                   kind in ['variable', 'typedef']:
                return
            #print('Entering here3')

            if self.include_function_definition:
                defn = first['definition'].firstChild.data
            else:
                defn = ""
            self.add_text('\n')
            briefd = node.getElementsByTagName('briefdescription');
            if kind == 'function' and briefd[0].firstChild.nodeValue == '\n':  # first node value if briefdescription exists will be always \n
                #print('Entering here4')
                #self.add_text('%csmethodmodifiers ')

                anc = node.parentNode.parentNode
                if cdef_kind in ('file', 'namespace'):
                    ns_node = anc.getElementsByTagName('innernamespace')
                    if not ns_node and cdef_kind == 'namespace':
                        ns_node = anc.getElementsByTagName('compoundname')
                    if ns_node:
                        ns = ns_node[0].firstChild.data
                        #print("himanshu ::::::   do_memberdef....ns_node")
                        self.add_text(' %s::%s "\n%s'%(ns, name, defn))
                    else:
                        #print("himanshu ::::::   do_memberdef....else")
                        #print name
                        #print("++++++++++++++++++++++++++++")
                        #print defn
                        self.add_text(name)
                        self.add_text(' \"')
                        self.add_text('\n')
                        self.add_text('/**\n')
                elif cdef_kind in ('class', 'struct'):
                    # Get the full function name.
                    anc_node = anc.getElementsByTagName('compoundname')
                    cname = anc_node[0].firstChild.data
                    #print("himanshu ::::::   do_memberdef...class/struct")

                    s = "Dali::Toolkit::"
                    s += name
                    b = "Dali::"
                    b += name
                    #print "himanshu ::::::   do_memberdef...class/struct %s" %b
                    if cname == s or cname == b:
                         #print("Inside %s "%s)
                         return
                    else:
                         self.add_text('%csmethodmodifiers ')
                         self.add_text([' %s::%s'%(cname, name)])
                         self.add_text(['\n','"\n/**\n'])

                for n in node.childNodes:
                    if n not in first.values():
                        self.parse(n)
                self.add_text(['\n','*/','\n','%s ";'%prot,'\n'])

    def do_definition(self, node):
        #print("himanshu ::::::   do_definition")
        data = node.firstChild.data
        self.add_text('%s "\n%s'%(data, data))

    def do_sectiondef(self, node):
        #print('Himanshu : ........SectionDef ........')
        kind = node.attributes['kind'].value
        #print('kind = %s'%kind)
        if kind in ('public-func', 'func', 'user-defined', 'public-type', ''):
            self.generic_parse(node)

    def do_header(self, node):
        """For a user defined section def a header field is present
        which should not be printed as such, so we comment it in the
        output."""
        data = node.firstChild.data
        self.add_text('\n/*\n %s \n*/\n'%data)
        # If our immediate sibling is a 'description' node then we
        # should comment that out also and remove it from the parent
        # node's children.
        parent = node.parentNode
        idx = parent.childNodes.index(node)
        if len(parent.childNodes) >= idx + 2:
            nd = parent.childNodes[idx+2]
            if nd.nodeName == 'description':
                nd = parent.removeChild(nd)
                self.add_text('\n/*')
                self.generic_parse(nd)
                self.add_text('\n*/\n')

    def do_parse_sect(self, node, kind):
        if kind in ('date', 'rcs', 'version'):
            pass
        elif kind == 'warning':
            self.add_text(['\n', '* @warning '])
            self.generic_parse(node,pad=0)
        elif kind == 'see':
            self.add_text('\n')
            self.add_text('* @see ')
            self.generic_parse(node,pad=0)
        elif kind == 'return':
            self.add_text('\n')
            self.add_text('* @return ')
            self.generic_parse(node,pad=0)
        elif kind == 'pre':
            self.add_text('\n')
            self.add_text('* @pre ')
            self.generic_parse(node,pad=0)
        elif kind == 'note':
            self.add_text('\n')
            self.add_text('* @note ')
            self.generic_parse(node,pad=0)
        elif kind == 'post':
            self.add_text('\n')
            self.add_text('* @post ')
            self.generic_parse(node,pad=0)
        elif kind == 'since':
            self.add_text('\n')
            self.add_text('* @SINCE_')
            self.generic_parse(node,pad=0)
        else:
            self.add_text('\n')
            self.generic_parse(node,pad=0)

    def do_simplesect(self, node):
        kind = node.attributes['kind'].value
        self.simplesect_kind = kind
        self.do_parse_sect(node, kind)
        self.simplesect_kind = ''

    def do_simplesectsep(self, node):
        #tmp = node.parentnode
        self.do_parse_sect(node, self.simplesect_kind)

    def do_argsstring(self, node):
        #self.generic_parse(node, pad=1)
        x = 0

    def do_member(self, node):
        kind = node.attributes['kind'].value
        refid = node.attributes['refid'].value
        if kind == 'function' and refid[:9] == 'namespace':
            self.generic_parse(node)

    def do_doxygenindex(self, node):
        self.multi = 1
        comps = node.getElementsByTagName('compound')
        for c in comps:
            refid = c.attributes['refid'].value
            fname = refid + '.xml'
            if not os.path.exists(fname):
                fname = os.path.join(self.my_dir,  fname)
            #if not self.quiet:
                #print "parsing file: %s"%fname
            p = Doxy2SWIG(fname, self.include_function_definition, self.quiet)
            p.generate()
            self.pieces.extend(self.clean_pieces(p.pieces))

    def do_emphasis(self,node):
        self.add_text('\n* <i> ')
        self.generic_parse(node,pad=0)
        self.add_text(' </i>')

    def do_heading(self,node):
        level = node.attributes['level'].value
        self.add_text('\n* <h%s> '%level)
        self.generic_parse(node,pad=0)
        self.add_text(' </h%s>\n* '%level)

    def do_itemizedlist(self, node):
        self.add_text(['\n* '])
        self.list_ctr = self.list_ctr + 2
        #self.firstListItem = self.firstListItem + 1
        self.generic_parse(node, pad=0)
        self.list_ctr = self.list_ctr - 2

    def do_listitem(self, node):
        #self.add_text('\n'* (self.firstListItem-1))
        #self.firstlistItem = self.firstListItem - 1
        self.add_text(' ' * self.list_ctr)
        self.add_text('- ')
        self.generic_parse(node, pad=0)

    def do_programlisting(self, node):
        self.add_text(['\n* '])
        self.add_text(' ' * (self.list_ctr+2))
        self.add_text('@code\n*')
        self.generic_parse(node, pad=0)
        self.add_text(' ' * (self.list_ctr+2))
        self.add_text('@endcode\n*')

    def do_codeline(self, node):
        self.add_text(' ' * (self.list_ctr+2))
        self.generic_parse(node, pad=1)

    def do_highlight(self, node):
        cl = node.attributes['class'].value
        self.add_text(' ')
        #if cl == 'normal':
        self.generic_parse(node, pad=0)

    def do_sp(self, node):
        self.add_text(' ')

    """def do_table(self, node);
        rows = node.attributes['rows'].value
        cols = node.attributes['cols'].value"""

    def do_enumvalue(self, node):
        self.generic_parse(node, pad=0)

    def write(self, fname):
        o = my_open_write(fname)
        if self.multi or 1:
            o.write(u"".join(self.pieces).encode('utf-8'))
        else:
            o.write("".join(self.clean_pieces(self.pieces)))
        o.close()

    def remove_trailing_spaces(self, fname):
        clean_lines = []
        with open(fname) as o:
            line = o.readlines()
            clean_lines = [l.strip() for l in line if l.strip()]

        with open('temp','w+') as f:
            f.writelines('\n'.join(clean_lines))

        f.close()
        """with open('temp','r+') as f:
            text = f.read()
        f.close()
        t = textwrap.fill(text, 100, break_long_words=False)
        t = t.replace('\n','\n* '+' '*(self.list_ctr+2))
        #t = t.replace('1234',' '*self.list_ctr)
        with open('temp','w+') as f:
            f.write(t)
        """
        os.rename('temp',fname)
        f.close()

    def clean_pieces(self, pieces):
        """Cleans the list of strings given as `pieces`.  It replaces
        multiple newlines by a maximum of 2 and returns a new list.
        It also wraps the paragraphs nicely.
        """
        ret = []
        count = 0
        for i in pieces:
            if i == '\n':
                count = count + 1
            else:
                if i == '";':
                    if count:
                        ret.append('\n')
                elif count > 2:
                    ret.append('\n\n')
                elif count:
                    ret.append('\n'*count)
                count = 0
                ret.append(i)

        _data = "".join(ret)
        ret = []
        for i in _data.split('\n\n'):
            if i == 'Parameters:' or i == 'Exceptions:':
                ret.extend([i, '\n-----------', '\n\n'])
            elif i.find('// File:') > -1: # leave comments alone.
                ret.extend([i, '\n'])
            else:
                _tmp = textwrap.fill(i.strip(), break_long_words=False)
                _tmp = self.lead_spc.sub(r'\1"\2', _tmp)
                ret.extend([_tmp, '\n\n'])
        return ret


def convert(input, output, include_function_definition=True, quiet=False):
    p = Doxy2SWIG(input, include_function_definition, quiet)
    p.generate()
    #p.pieces=[str(i) for i in p.pieces]
    #print p.pieces
    p.write(output)
    p.remove_trailing_spaces(output)

def main():
    usage = __doc__
    parser = optparse.OptionParser(usage)
    parser.add_option("-n", '--no-function-definition',
                      action='store_true',
                      default=False,
                      dest='func_def',
                      help='do not include doxygen function definitions')
    parser.add_option("-q", '--quiet',
                      action='store_true',
                      default=False,
                      dest='quiet',
                      help='be quiet and minimize output')

    options, args = parser.parse_args()
    if len(args) != 2:
        parser.error("error: no input and output specified")

    convert(args[0], args[1], not options.func_def, options.quiet)


if __name__ == '__main__':
    main()