// --------------------------------------------------------------------------
//                   OpenMS -- Open-Source Mass Spectrometry
// --------------------------------------------------------------------------
// Copyright The OpenMS Team -- Eberhard Karls University Tuebingen,
// ETH Zurich, and Freie Universitaet Berlin 2002-2021.
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
// $Maintainer: Hendrik Weisser $
// $Authors: Marc Sturm, Clemens Groepl, Hendrik Weisser $
// --------------------------------------------------------------------------

#include <OpenMS/APPLICATIONS/MapAlignerBase.h>

using namespace OpenMS;
using namespace std;

//-------------------------------------------------------------
// Doxygen docu
//-------------------------------------------------------------

/**
    @page TOPP_MapRTTransformer MapRTTransformer

    @brief Applies retention time transformations to maps.

<CENTER>
    <table>
        <tr>
            <td ALIGN = "center" BGCOLOR="#EBEBEB"> potential predecessor tools </td>
            <td VALIGN="middle" ROWSPAN=2> \f$ \longrightarrow \f$ MapRTTransformer \f$ \longrightarrow \f$</td>
            <td ALIGN = "center" BGCOLOR="#EBEBEB"> potential successor tools </td>
        </tr>
        <tr>
            <td VALIGN="middle" ALIGN = "center" ROWSPAN=1> @ref TOPP_MapAlignerIdentification @n (or another alignment algorithm) </td>
            <td VALIGN="middle" ALIGN = "center" ROWSPAN=1> @ref TOPP_FeatureLinkerUnlabeled or @n @ref TOPP_FeatureLinkerUnlabeledQT </td>
        </tr>
    </table>
</CENTER>

    This tool can apply retention time transformations to different types of data (mzML, featureXML, consensusXML, and idXML files).
    The transformations might have been generated by a previous invocation of one of the MapAligner tools (linked below).
    However, the trafoXML file format is not very complicated, so it is relatively easy to write (or generate) your own files.
    Each input file will give rise to one output file.

    @see @ref TOPP_MapAlignerIdentification @ref TOPP_MapAlignerPoseClustering @ref TOPP_MapAlignerSpectrum

    With this tool it is also possible to invert transformations, or to fit a different model than originally specified to the retention time data in the transformation files. To fit a new model, choose a value other than "none" for the model type (see below).

    Original retention time values can be kept as meta data. With the option @p store_original_rt, meta values with the name "original_RT" and the original retention time will be created for every major data element (spectrum, chromatogram, feature, consensus feature, peptide identification), unless they already exist - "original_RT" values from a previous invocation will not be overwritten.

    Since %OpenMS 1.8, the extraction of data for the alignment has been separate from the modeling of RT transformations based on that data. It is now possible to use different models independently of the chosen algorithm. The different available models are:
    - @ref OpenMS::TransformationModelLinear "linear": Linear model.
    - @ref OpenMS::TransformationModelBSpline "b_spline": Smoothing spline (non-linear).
    - @ref OpenMS::TransformationModelInterpolated "interpolated": Different types of interpolation.

    The following parameters control the modeling of RT transformations (they can be set in the "model" section of the INI file):
    @htmlinclude OpenMS_MapRTTransformerModel.parameters @n

    @note As output options, either @p out or @p trafo_out has to be provided. They can be used together.

    @note Currently mzIdentML (mzid) is not directly supported as an input/output format of this tool. Convert mzid files to/from idXML using @ref TOPP_IDFileConverter if necessary.

    <B>The command line parameters of this tool are:</B> @n
    @verbinclude TOPP_MapRTTransformer.cli
    <B>INI file documentation of this tool:</B>
    @htmlinclude TOPP_MapRTTransformer.html

*/

// We do not want this class to show up in the docu:
/// @cond TOPPCLASSES

class TOPPMapRTTransformer :
  public TOPPBase
{

public:
  TOPPMapRTTransformer() :
    TOPPBase("MapRTTransformer", "Applies retention time transformations to maps.")
  {
  }

protected:
  void registerOptionsAndFlags_() override
  {
    String file_formats = "mzML,featureXML,consensusXML,idXML";
    // "in" is not required, in case we only want to invert a transformation:
    registerInputFile_("in", "<file>", "", "Input file to transform (separated by blanks)", false);
    setValidFormats_("in", ListUtils::create<String>(file_formats));
    registerOutputFile_("out", "<file>", "", "Output file (same file type as 'in'). This option or 'trafo_out' has to be provided; they can be used together.", false);
    setValidFormats_("out", ListUtils::create<String>(file_formats));
    registerInputFile_("trafo_in", "<file>", "", "Transformation to apply");
    setValidFormats_("trafo_in", ListUtils::create<String>("trafoXML"));
    registerOutputFile_("trafo_out", "<file>", "", "Transformation output file. This option or 'out' has to be provided; they can be used together.", false);
    setValidFormats_("trafo_out", ListUtils::create<String>("trafoXML"));
    registerFlag_("invert", "Invert transformation (approximatively) before applying it");
    registerFlag_("store_original_rt", "Store the original retention times (before transformation) as meta data in the output file");
    addEmptyLine_();

    registerSubsection_("model", "Options to control the modeling of retention time transformations from data");
  }

  Param getSubsectionDefaults_(const String& /* section */) const override
  {
    return TOPPMapAlignerBase::getModelDefaults("none");
  }

  template <class TFile, class TMap>
  void applyTransformation_(const String& in, const String& out, 
                            const TransformationDescription& trafo,
                            TFile& file, TMap& map)
  {
    file.load(in, map);
    bool store_original_rt = getFlag_("store_original_rt");
    MapAlignmentTransformer::transformRetentionTimes(map, trafo,
                                                     store_original_rt);
    addDataProcessing_(map, getProcessingInfo_(DataProcessing::ALIGNMENT));
    file.store(out, map);
  }

  ExitCodes main_(int, const char**) override
  {
    //-------------------------------------------------------------
    // parameter handling
    //-------------------------------------------------------------
    String in = getStringOption_("in");
    String out = getStringOption_("out");
    String trafo_in = getStringOption_("trafo_in");
    String trafo_out = getStringOption_("trafo_out");
    Param model_params = getParam_().copy("model:", true);
    String model_type = model_params.getValue("type").toString();
    model_params = model_params.copy(model_type + ":", true);

    ProgressLogger progresslogger;
    progresslogger.setLogType(log_type_);

    //-------------------------------------------------------------
    // check for valid input
    //-------------------------------------------------------------
    if (out.empty() && trafo_out.empty())
    {
      writeLog_("Error: A data or a transformation output file has to be provided (parameters 'out'/'trafo_out')");
      return ILLEGAL_PARAMETERS;
    }
    if (in.empty() != out.empty())
    {
      writeLog_("Error: Data input and output parameters ('in'/'out') must be used together");
      return ILLEGAL_PARAMETERS;
    }

    //-------------------------------------------------------------
    // apply transformation
    //-------------------------------------------------------------
    TransformationXMLFile trafoxml;
    TransformationDescription trafo;
    trafoxml.load(trafo_in, trafo);
    if (model_type != "none")
    {
      trafo.fitModel(model_type, model_params);
    }
    if (getFlag_("invert"))
    {
      trafo.invert();
    }
    if (!trafo_out.empty())
    {
      trafoxml.store(trafo_out, trafo);
    }
    if (!in.empty()) // load input
    {
      FileTypes::Type in_type = FileHandler::getType(in);
      if (in_type == FileTypes::MZML)
      {
        MzMLFile file;
        PeakMap map;
        applyTransformation_(in, out, trafo, file, map);
      }
      else if (in_type == FileTypes::FEATUREXML)
      {
        FeatureXMLFile file;
        FeatureMap map;
        applyTransformation_(in, out, trafo, file, map);
      }
      else if (in_type == FileTypes::CONSENSUSXML)
      {
        ConsensusXMLFile file;
        ConsensusMap map;
        applyTransformation_(in, out, trafo, file, map);
      }
      else if (in_type == FileTypes::IDXML)
      {
        IdXMLFile file;
        vector<ProteinIdentification> proteins;
        vector<PeptideIdentification> peptides;
        file.load(in, proteins, peptides);
        bool store_original_rt = getFlag_("store_original_rt");
        MapAlignmentTransformer::transformRetentionTimes(peptides, trafo,
                                                         store_original_rt);
        // no "data processing" section in idXML
        file.store(out, proteins, peptides);
      }
    }

    return EXECUTION_OK;
  }

};


int main(int argc, const char** argv)
{
  TOPPMapRTTransformer tool;
  return tool.main(argc, argv);
}

/// @endcond
