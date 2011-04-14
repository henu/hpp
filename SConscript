import glob

# Define used directories
srcDir = '.'
includeDir = '.'
libDir = '.'

#directories = [includeDir]
#directories += [includeDir+'/Dahlia']
#directories += ['../GLee/include']

env = Environment() #CPPPATH = directories)

# Get compiler and linker flags for SDL
env.ParseConfig('sdl-config --cflags')
env.ParseConfig('sdl-config --libs')

# Define all sourcefiles
SOURCES =  glob.glob(srcDir+'/*.cc')


# Additional compiler flags
env.Append(CCFLAGS = ['-Wall'])

# - Release
env.Append(CCFLAGS = ['-O3'])

# - Debug
#env.Append(CCFLAGS = ['-g'])
#env.Append(CCFLAGS = ['-g', '-pg'])
#env.Append(LINKFLAGS = ['-pg'])

# Additional libraries to link against
env.Append(LIBS = ['ncursesw'])
env.Append(LIBS = ['GL'])
env.Append(LIBS = ['SDL_image'])

# Define the build target
# - Output library will be "libDahlia"
env.StaticLibrary(target = libDir+'/hpp', source = SOURCES)
#env.SharedLibrary(target = libDir+'/hpp', source = SOURCES)

