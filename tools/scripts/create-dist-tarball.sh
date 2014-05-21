#!/bin/sh
# sanitize working directory:
mydir=`dirname "$0"`
cd "$mydir"
mydir=`pwd -L`

cd "$mydir/../.."

ARGS=
for arg
do
	case "$arg" in
		-h|--help) 
			echo "Usage: $0 [--export-only] [--no-delete|"
			echo "Creates an exported source tree tarball."
			echo "If --no-delete is given, the exported source tree is not deleted after tarball creation."
			echo "If --export-only is given, no tarball is created (implies --no-delete)."
			exit 0
			;;
		--no-delete)
			ARGS="$ARGS -DNO_DELETE_EXPORT_TREE=TRUE"
			;;
		--export-only)
			ARGS="-DNO_DELETE_EXPORT_TREE=TRUE -DNO_TARBALL=TRUE"
	esac
done

echo cmake $ARGS -P "$mydir/create-dist-tarball.cmake"
cmake $ARGS -P "$mydir/create-dist-tarball.cmake"
