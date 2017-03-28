// --------------------------------------------------------------------------
//                   OpenMS -- Open-Source Mass Spectrometry
// --------------------------------------------------------------------------
// Copyright The OpenMS Team -- Eberhard Karls University Tuebingen,
// ETH Zurich, and Freie Universitaet Berlin 2002-2016.
//
// This software is released under a three-clause BSD license:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of any author or any participating institution
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// For a full list of authors, refer to the file AUTHORS.
// --------------------------------------------------------------------------
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Lukas Zimmermann $
// $Authors: Lukas Zimmermann $
// --------------------------------------------------------------------------
#ifndef OPENMS_FORMAT_XQUESTRESULTXMLFILE_H
#define OPENMS_FORMAT_XQUESTRESULTXMLFILE_H

#include <OpenMS/FORMAT/XMLFile.h>
#include <OpenMS/METADATA/XQuestResultMeta.h>
#include <OpenMS/METADATA/PeptideIdentification.h>
#include <OpenMS/METADATA/ProteinIdentification.h>

namespace OpenMS
{

  class OPENMS_DLLAPI XQuestResultXMLFile :
    public Internal::XMLFile
  {
public:
    XQuestResultXMLFile();
    ~XQuestResultXMLFile();

    void load(const String &,  // filename
              std::vector< XQuestResultMeta >  &, // Vector to be filled with Metadata about individual XQuest results
              std::vector< std::vector< PeptideIdentification > > &, // Vector of encompassed spectra, each containing associated PeptideIdentifications
              bool = false,  // Whether or not the cumnulated hit no. over the spectra should be calculated
              size_t = 0,  // Minimum number of PeptideIdentification a spectrum must contain to be loaded
              bool = false); // Whether the OpenXQuest data will be loaded as Meta Values also into the peptide hits, instead just into the PeptideIdentification
    
    void store(const String &, std::vector< std::vector< PeptideIdentification > > & );

    int get_n_hits() const;
    std::vector < int > * get_cum_hits() const;

    void delete_cum_hits();

private:
    int n_hits_; // Total number of hits within the result file
    std::vector< int > * cum_hits_; // Points to vector with cumulated the number of hits per spectrum in the order they appear in the file
  };
} // namespace OpenMS
#endif // OPENMS_FORMAT_XQUESTRESULTXMLFILE_H
