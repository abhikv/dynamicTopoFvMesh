/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    convexSetAlgorithm

Description
    Base class for convexSetAlgorithms

Author
    Sandeep Menon
    University of Massachusetts Amherst
    All rights reserved

SourceFiles
    convexSetAlgorithm.C

\*---------------------------------------------------------------------------*/

#ifndef convexSetAlgorithm_H
#define convexSetAlgorithm_H

#include "Map.H"
#include "label.H"
#include "edgeList.H"
#include "faceList.H"
#include "cellList.H"
#include "objectMap.H"
#include "vectorField.H"

// Use the MPFR library, if necessary
#define USE_MPFR 1

#if USE_MPFR
#include "multiPrecision.H"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class polyMesh;

/*---------------------------------------------------------------------------*\
                    Class convexSetAlgorithm Declaration
\*---------------------------------------------------------------------------*/

class convexSetAlgorithm
{

protected:

    // Protected data

        const bool twoDMesh_;
        const label nOldPoints_;

        //- References to old-level connectivity
        //  [Before topo-changes, at old point-positions]
        const polyMesh& mesh_;

        //- References to new-level connectivity
        //  [After topo-changes, at old point-positions]
        const pointField& newPoints_;
        const UList<edge>& newEdges_;
        const UList<face>& newFaces_;
        const UList<cell>& newCells_;
        const UList<label>& newOwner_;
        const UList<label>& newNeighbour_;

        //- Reference to pointsFromPoints information
        const List<objectMap>& pointsFromPoints_;

        //- Reference to modified-points information
        const Map<labelList>& modPoints_;

        //- Switch for higher precision
        mutable bool highPrecision_;

        //- Entity parents
        mutable labelList parents_;

        //- Low-precision fields
        mutable vector refNorm_;
        mutable scalar normFactor_;
        mutable vectorField centres_;
        mutable scalarField weights_;

#       if USE_MPFR

        //- High-precision fields
        mutable mpVector mpRefNorm_;
        mutable mpScalar mpNormFactor_;
        mutable Field<mpVector> mpCentres_;
        mutable Field<mpScalar> mpWeights_;

#       endif

    //- Output connectivity to VTK

        // Output an entity as a VTK file
        void writeVTK
        (
            const word& name,
            const label entity,
            const label primitiveType = 3,
            const bool useOldConnectivity = false
        ) const;

        // Output a list of entities as a VTK file
        void writeVTK
        (
            const word& name,
            const labelList& cList,
            const label primitiveType = 3,
            const bool useOldConnectivity = false
        ) const;

public:

    //- Constructor

        // Construct from components
        convexSetAlgorithm
        (
            const polyMesh& mesh,
            const pointField& newPoints,
            const UList<edge>& newEdges,
            const UList<face>& newFaces,
            const UList<cell>& newCells,
            const UList<label>& newOwner,
            const UList<label>& newNeighbour,
            const List<objectMap>& pointsFromPoints,
            const Map<labelList>& modPoints
        );

    //- Destructor

        virtual ~convexSetAlgorithm()
        {}

    //- Member functions

        // Dimensions of the algorithm
        virtual label dimension() const = 0;

        // Return true if accumulated weights are consistent
        virtual bool consistent(const scalar tolerance) const;

        // Return the normFactor
        virtual scalar normFactor() const;

        // Normalize stored weights
        virtual void normalize(bool normSum) const;

        // Extract weights and centres to lists
        virtual void populateLists
        (
            labelList& parents,
            vectorField& centres,
            scalarField& weights
        ) const;

        // Return the highPrecision switch
        bool highPrecision() const;

        // Set higher precision mode
        void setHighPrecision() const;

        // Reset higher precision mode
        void unsetHighPrecision() const;

        // Compute normFactor
        virtual void computeNormFactor(const label index) const = 0;

        // Compute intersections
        virtual bool computeInsersection
        (
            const label newIndex,
            const label oldIndex,
            const scalar& matchTol,
            bool output
        ) const = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //