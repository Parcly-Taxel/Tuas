project('list5', 'c')

gtkdep = dependency('gtk4')

gnome=import('gnome')
resources = gnome.compile_resources('resources','window.gresource.xml')

sourcefiles=files('window.c')

executable('window', sourcefiles, resources, dependencies: gtkdep, export_dynamic: true, install: false)

