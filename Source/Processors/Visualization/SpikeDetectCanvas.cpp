/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2013 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "SpikeDetectCanvas.h"
#include "SpikeDisplayCanvas.h"

SpikeDetectCanvas::SpikeDetectCanvas(SpikeDetector* n) :
    processor(n), newSpike(false)
{
	electrode = nullptr;
    viewport = new Viewport();
    spikeDisplay = new SpikeThresholdDisplay(n,this, viewport);

    viewport->setViewedComponent(spikeDisplay, false);
    viewport->setScrollBarsShown(true, true);

	inDrawingPolygonMode = false;
    scrollBarThickness = viewport->getScrollBarThickness();

	/*
    clearButton = new UtilityButton("Clear plots", Font("Small Text", 13, Font::plain));
    clearButton->setRadius(3.0f);
    clearButton->addListener(this);
    addAndMakeVisible(clearButton);
	*/


    addUnitButton = new UtilityButton("Add box unit", Font("Small Text", 13, Font::plain));
    addUnitButton->setRadius(3.0f);
    addUnitButton->addListener(this);
    addAndMakeVisible(addUnitButton);
  
    addPolygonUnitButton = new UtilityButton("Add polygon unit", Font("Small Text", 13, Font::plain));
    addPolygonUnitButton->setRadius(3.0f);
    addPolygonUnitButton->addListener(this);
    addAndMakeVisible(addPolygonUnitButton);
 
    delUnitButton = new UtilityButton("Del unit", Font("Small Text", 13, Font::plain));
    delUnitButton->setRadius(3.0f);
    delUnitButton->addListener(this);
    addAndMakeVisible(delUnitButton);
  
    addBoxButton = new UtilityButton("Add box", Font("Small Text", 13, Font::plain));
    addBoxButton->setRadius(3.0f);
    addBoxButton->addListener(this);
    addAndMakeVisible(addBoxButton);
  
    delBoxButton = new UtilityButton("Del box", Font("Small Text", 13, Font::plain));
    addBoxButton->setRadius(3.0f);
    delBoxButton->addListener(this);
    addAndMakeVisible(delBoxButton);
 
    rePCAButton = new UtilityButton("Re-PCA", Font("Small Text", 13, Font::plain));
    rePCAButton->setRadius(3.0f);
    rePCAButton->addListener(this);
    addAndMakeVisible(rePCAButton);
 

    addAndMakeVisible(viewport);

    setWantsKeyboardFocus(true);

    update();
	 
}

SpikeDetectCanvas::~SpikeDetectCanvas()
{
//    processor->removeSpikePlots();
}

void SpikeDetectCanvas::beginAnimation()
{
    std::cout << "SpikeDetectCanvas beginning animation." << std::endl;

    startCallbacks();
}

void SpikeDetectCanvas::endAnimation()
{
    std::cout << "SpikeDetectCanvas ending animation." << std::endl;

    stopCallbacks();
}

void SpikeDetectCanvas::update()
{

    std::cout << "Updating SpikeDetectCanvas" << std::endl;

    int nPlots = processor->getNumElectrodes();
    processor->removeSpikePlots();
    spikeDisplay->removePlots();

	if (nPlots > 0)  {
		// Plot only active electrode
		int currentElectrode = processor->getCurrentElectrodeIndex();
			electrode = processor->getActiveElectrode();
			SpikeHistogramPlot* sp = spikeDisplay->addSpikePlot(processor->getNumberOfChannelsForElectrode(currentElectrode), currentElectrode,
				processor->getNameForElectrode(currentElectrode));
			processor->addSpikePlotForElectrode(sp, currentElectrode);
			electrode->spikePlot->updateUnitsFromProcessor();
		
	}
    spikeDisplay->resized();
    spikeDisplay->repaint();
}


void SpikeDetectCanvas::refreshState()
{
    // called when the component's tab becomes visible again
    resized();
}

void SpikeDetectCanvas::resized()
{
    viewport->setBounds(0,0,getWidth(),getHeight()-20);

    spikeDisplay->setBounds(0,0,getWidth()-scrollBarThickness, spikeDisplay->getTotalHeight());

    //clearButton->setBounds(10, getHeight()-20, 100,20);
	addUnitButton->setBounds(10, getHeight()-20, 100,20);
	addPolygonUnitButton->setBounds(150, getHeight()-20, 150,20);
	delUnitButton->setBounds(350, getHeight()-20, 100,20);
	addBoxButton->setBounds(500, getHeight()-20, 100,20);
	delBoxButton->setBounds(650, getHeight()-20, 100,20);
	rePCAButton->setBounds(800, getHeight()-20, 100,20);
	
}

void SpikeDetectCanvas::paint(Graphics& g)
{

    g.fillAll(Colours::darkgrey);

}

void SpikeDetectCanvas::refresh()
{
	// called every 10 Hz
    processSpikeEvents();

    repaint();
}


void SpikeDetectCanvas::processSpikeEvents()
{


	Electrode *e = ((SpikeDetector*)processor)->getActiveElectrode();

//	e->spikeSort->lstLastSpikes
//    processor->setParameter(2, 0.0f); // request redraw

}

bool SpikeDetectCanvas::keyPressed(const KeyPress& key)
{

    KeyPress c = KeyPress::createFromDescription("c");

	 KeyPress e = KeyPress::createFromDescription("escape");
        
    if (key.isKeyCode(c.getKeyCode())) // C
    {
        spikeDisplay->clear();

        std::cout << "Clearing display" << std::endl;
        return true;
    }
	      
    if (key.isKeyCode(e.getKeyCode())) // C
    {
        spikeDisplay->setPolygonMode(false);
        return true;
    }

    return false;

}

void SpikeDetectCanvas::buttonClicked(Button* button)
{
		int channel = 0;
		int unitID = -1;
		int boxID = -1;
		Time t;
    if (button == addPolygonUnitButton)
    {
        inDrawingPolygonMode = true;
		addPolygonUnitButton->setToggleState(true,false);
		electrode->spikePlot->setPolygonDrawingMode(true);
	} else if (button == addUnitButton)
	{
		Electrode *e = processor->getActiveElectrode();
		if (e != nullptr) {
			int electrodeID = processor->getActiveElectrode()->electrodeID;
			int newUnitID = processor->getActiveElectrode()->spikeSort->addBoxUnit(0);
			uint8 r,g,b;
			processor->getActiveElectrode()->spikeSort->getUnitColor(newUnitID, r,g,b);
			electrode->spikePlot->updateUnitsFromProcessor();
			electrode->spikePlot->setSelectedUnitAndbox(newUnitID,0);
			String eventlog = "NewUnit "+String(electrodeID) + " "+String(newUnitID)+" "+String(r)+" "+String(g)+" "+String(b);
			processor->addNetworkEventToQueue(StringTS(eventlog));
		}

	}else if (button == delUnitButton)
	{
		int electrodeID = processor->getActiveElectrode()->electrodeID;
		
		processor->getActiveElectrode()->spikePlot->getSelectedUnitAndbox(unitID, boxID);
		if (unitID > 0)
		{
			processor->getActiveElectrode()->spikeSort->removeUnit(unitID);
			electrode->spikePlot->updateUnitsFromProcessor();

			String eventlog = "RemoveUnit "+String(electrodeID) + " "+String(unitID);
			processor->addNetworkEventToQueue(StringTS(eventlog));

			// set new selected unit to be the last existing unit
			std::vector<BoxUnit> u = processor->getActiveElectrode()->spikeSort->getBoxUnits();
			if (u.size() > 0)
			{
				electrode->spikePlot->setSelectedUnitAndbox(u[u.size()-1].getUnitID(),0);
			} else 
			{
				electrode->spikePlot->setSelectedUnitAndbox(-1,-1);
			}

		}
	} else if (button == addBoxButton)
	{
		processor->getActiveElectrode()->spikePlot->getSelectedUnitAndbox(unitID, boxID);
		if (unitID > 0) 
		{
			processor->getActiveElectrode()->spikeSort->addBoxToUnit(channel,unitID);
			electrode->spikePlot->updateUnitsFromProcessor();
		}
	}
	 else if (button == delBoxButton)
	{
	processor->getActiveElectrode()->spikePlot->getSelectedUnitAndbox(unitID, boxID);
		if (unitID > 0) 
		{
			processor->getActiveElectrode()->spikeSort->removeBoxFromUnit(unitID, boxID);
			electrode->spikePlot->updateUnitsFromProcessor();
		}
	} else if (button == rePCAButton)
	{
		processor->getActiveElectrode()->spikeSort->RePCA();
	}
	repaint();

}





// ----------------------------------------------------------------

SpikeThresholdDisplay::SpikeThresholdDisplay(SpikeDetector *p, SpikeDetectCanvas* sdc, Viewport* v) :
    processor(p),canvas(sdc), viewport(v)
{

    totalHeight = 1000;

}

SpikeThresholdDisplay::~SpikeThresholdDisplay()
{

}

void SpikeThresholdDisplay::clear()
{
    if (spikePlots.size() > 0)
    {
        for (int i = 0; i < spikePlots.size(); i++)
        {
            spikePlots[i]->clear();
        }
    }

}


void SpikeThresholdDisplay::removePlots()
{
    spikePlots.clear();

}

SpikeHistogramPlot* SpikeThresholdDisplay::addSpikePlot(int numChannels, int electrodeNum, String name_)
{

    std::cout << "Adding new spike plot." << std::endl;

    SpikeHistogramPlot* spikePlot = new SpikeHistogramPlot(processor,canvas, electrodeNum, 1000 + numChannels, name_);
    spikePlots.add(spikePlot);
    addAndMakeVisible(spikePlot);

    return spikePlot;
}

void SpikeThresholdDisplay::paint(Graphics& g)
{

    g.fillAll(Colours::grey);

}

void SpikeThresholdDisplay::setPolygonMode(bool on)
{
  if (spikePlots.size() > 0)
	  spikePlots[0]->setPolygonDrawingMode(on);
}

void SpikeThresholdDisplay::resized()
{
    // this is kind of a mess -- is there any way to optimize it?

    if (spikePlots.size() > 0)
    {
			
        int w = getWidth();
		int h = 430;//getHeight();
		
			spikePlots[0]->setBounds(0, 0, w, h);

    
        setBounds(0, 0, w, h);
    }

}

void SpikeThresholdDisplay::mouseDown(const MouseEvent& event)
{
	
}

void SpikeThresholdDisplay::plotSpike(const SpikeObject& spike, int electrodeNum)
{
    spikePlots[electrodeNum]->processSpikeObject(spike);

}






// ----------------------------------------------------------------

SpikeHistogramPlot::SpikeHistogramPlot(SpikeDetector *prc,SpikeDetectCanvas* sdc, int elecNum, int p, String name_) :
    processor(prc), canvas(sdc), isSelected(false), electrodeNumber(elecNum),  plotType(p),
    limitsChanged(true), name(name_)

{
	
    font = Font("Default", 15, Font::plain);

    switch (p)
    {
        case SINGLE_PLOT:
            // std::cout<<"SpikePlot as SINGLE_PLOT"<<std::endl;
            nWaveAx = 1;
            nProjAx = 1;
            nChannels = 1;
            minWidth = 600;
            aspectRatio = 0.5f;
            break;
        case STEREO_PLOT:
            //  std::cout<<"SpikePlot as STEREO_PLOT"<<std::endl;
            nWaveAx = 2;
            nProjAx = 1;
            nChannels = 2;
            minWidth = 300;
            aspectRatio = 0.5f;
            break;
        case TETRODE_PLOT:
            // std::cout<<"SpikePlot as TETRODE_PLOT"<<std::endl;
            nWaveAx = 4;
            nProjAx = 1;
            nChannels = 4;
            minWidth = 400;
            aspectRatio = 0.5f;
            break;
            //        case HIST_PLOT:
            //            nWaveAx = 1;
            //            nProjAx = 0;
            //            nHistAx = 1;
            //            break;
        default: // unsupported number of axes provided
            std::cout << "SpikePlot as UNKNOWN, defaulting to SINGLE_PLOT" << std::endl;
            nWaveAx = 1;
            nProjAx = 0;
            plotType = SINGLE_PLOT;
            nChannels = 1;
    }

    initAxes();

    for (int i = 0; i < nChannels; i++)
    {
        UtilityButton* rangeButton = new UtilityButton("250", Font("Small Text", 10, Font::plain));
        rangeButton->setRadius(3.0f);
        rangeButton->addListener(this);
        addAndMakeVisible(rangeButton);

        rangeButtons.add(rangeButton);
    }

}


void SpikeHistogramPlot::setSelectedUnitAndbox(int unitID, int boxID)
{
	mut.enter();
	processor->getActiveElectrode()->spikeSort->setSelectedUnitAndbox(unitID, boxID);
	/*if (nWaveAx > 0) 
	{
		 wAxes[0]->selectedUnit = unitID;
		 wAxes[0]->selectedBox = boxID ;
	}*/
	mut.exit();
}

void SpikeHistogramPlot::getSelectedUnitAndbox(int &unitID, int &boxID)
{
	mut.enter();
	processor->getActiveElectrode()->spikeSort->getSelectedUnitAndbox(unitID, boxID);
	mut.exit();
}


SpikeHistogramPlot::~SpikeHistogramPlot()
{

}

void SpikeHistogramPlot::paint(Graphics& g)
{
    
    //const MessageManagerLock mmLock;

    g.setColour(Colours::white);
    g.drawRect(0,0,getWidth(),getHeight());

    g.setFont(font);

    g.drawText(name,10,0,200,20,Justification::left,false);
    
}

void SpikeHistogramPlot::setPolygonDrawingMode(bool on)
{
	mut.enter();
	pAxes[0]->setPolygonDrawingMode(on);
	mut.exit();
}

void SpikeHistogramPlot::updateUnitsFromProcessor()
{
	mut.enter();
	boxUnits = processor->getActiveElectrode()->spikeSort->getBoxUnits();
	pcaUnits = processor->getActiveElectrode()->spikeSort->getPCAUnits();

	if (nWaveAx > 0) 
	{
		wAxes[0]->updateUnits(boxUnits);
	}
	pAxes[0]->updateUnits(pcaUnits);
	mut.exit();

}

void SpikeHistogramPlot::setPCARange(float p1min, float p2min, float p1max, float p2max)
{
	mut.enter();
	pAxes[0]->setPCARange(p1min, p2min, p1max, p2max);
	mut.exit();

}

void SpikeHistogramPlot::processSpikeObject(const SpikeObject& s)
{
	mut.enter();
	if (nWaveAx > 0) 
	{
        for (int i = 0; i < nWaveAx; i++)
		{
            wAxes[i]->updateSpikeData(s);
		}

		pAxes[0]->updateSpikeData(s);
		
	}
mut.exit();
}

void SpikeHistogramPlot::select()
{
    isSelected = true;
}

void SpikeHistogramPlot::deselect()
{
    isSelected = false;
}

void SpikeHistogramPlot::initAxes()
{
	mut.enter();
    initLimits();

    for (int i = 0; i < nWaveAx; i++)
    {
        WaveformAxes* wAx = new WaveformAxes(processor, i);
		wAx->setDetectorThreshold( processor->getActiveElectrode()->thresholds[i]);
        wAxes.add(wAx);
        addAndMakeVisible(wAx);
        ranges.add(250.0f); // default range is 250 microvolts
    }

	PCAProjectionAxes* pAx = new PCAProjectionAxes(processor);
	float p1min,p2min, p1max,  p2max;
	processor->getActiveElectrode()->spikeSort->getPCArange(p1min,p2min, p1max,  p2max);
	pAx->setPCARange(p1min,p2min, p1max,  p2max);

    pAxes.add(pAx);
    addAndMakeVisible(pAx);

    setLimitsOnAxes(); // initialize the ranges
	mut.exit();
}

void SpikeHistogramPlot::resized()
{
	mut.enter();

    float width = getWidth()-10;
    float height = getHeight()-25;

    float axesWidth, axesHeight;

    // to compute the axes positions we need to know how many columns of proj and wave axes should exist
    // using these two values we can calculate the positions of all of the sub axes
    int nProjCols, nWaveCols;

    switch (plotType)
    {
        case SINGLE_PLOT:
            nProjCols = 1;
            nWaveCols = 1;
            axesWidth = width/2;
            axesHeight = height;
            break;

        case STEREO_PLOT:
            nProjCols = 1;
            nWaveCols = 2;
            axesWidth = width/2;
            axesHeight = height;
            break;
        case TETRODE_PLOT:
            nProjCols = 1;
            nWaveCols = 2;
            axesWidth = width/2;
            axesHeight = height/2;
            break;
    }

    for (int i = 0; i < nWaveAx; i++)
    {
        wAxes[i]->setBounds(5 + (i % nWaveCols) * axesWidth/nWaveCols, 20 + (i/nWaveCols) * axesHeight, axesWidth/nWaveCols, axesHeight);
        rangeButtons[i]->setBounds(8 + (i % nWaveCols) * axesWidth/nWaveCols,
                                   20 + (i/nWaveCols) * axesHeight + axesHeight - 18,
                                   25, 15);
    }

   // for (int i = 0; i < nProjAx; i++)
        pAxes[0]->setBounds(5 +  axesWidth, 20 + 0, width/2, height);

	mut.exit();

}

void SpikeHistogramPlot::buttonClicked(Button* button)
{
    UtilityButton* buttonThatWasClicked = (UtilityButton*) button;

    int index = rangeButtons.indexOf(buttonThatWasClicked);
    String label;

    if (ranges[index] == 250.0f)
    {
        ranges.set(index, 500.0f);
        label = "500";
    }
    else if (ranges[index] == 500.0f)
    {
        ranges.set(index, 100.0f);
        label = "100";
    }
    else if (ranges[index] == 100.0f)
    {
        ranges.set(index, 250.0f);
        label = "250";
    }

    buttonThatWasClicked->setLabel(label);

    setLimitsOnAxes();

}

void SpikeHistogramPlot::setLimitsOnAxes()
{
    //std::cout<<"SpikePlot::setLimitsOnAxes()"<<std::endl;
	mut.enter();
    for (int i = 0; i < nWaveAx; i++)
        wAxes[i]->setRange(ranges[i]);

    
	mut.exit();
}

void SpikeHistogramPlot::initLimits()
{
    for (int i = 0; i < nChannels; i++)
    {
        limits[i][0] = 1209;//-1*pow(2,11);
        limits[i][1] = 11059;//pow(2,14)*1.6;
    }

}

void SpikeHistogramPlot::getBestDimensions(int* w, int* h)
{
    switch (plotType)
    {
        case TETRODE_PLOT:
            *w = 4;
            *h = 2;
            break;
        case STEREO_PLOT:
            *w = 2;
            *h = 1;
            break;
        case SINGLE_PLOT:
            *w = 1;
            *h = 1;
            break;
        default:
            *w = 1;
            *h = 1;
            break;
    }
}

void SpikeHistogramPlot::clear()
{
	mut.enter();
    std::cout << "SpikePlot::clear()" << std::endl;

    for (int i = 0; i < nWaveAx; i++)
        wAxes[i]->clear();
    for (int i = 0; i < nProjAx; i++)
        pAxes[i]->clear();

	mut.exit();
}


void SpikeHistogramPlot::setDisplayThresholdForChannel(int i, float f)
{
	mut.enter();
	if (i < wAxes.size())
		wAxes[i]->setDetectorThreshold(f);
	mut.exit();
	return;
}


float SpikeHistogramPlot::getDisplayThresholdForChannel(int i)
{
	mut.enter();
    float f= wAxes[i]->getDisplayThreshold();
	mut.exit();
	return f;
}

/*
void SpikeHistogramPlot::setDetectorThresholdForChannel(int i, float t)
{
   // std::cout << "Setting threshold to " << t << std::endl;
	mut.enter();
    wAxes[i]->setDetectorThreshold(t);
	mut.exit();
}
*/

/********************************/



// --------------------------------------------------

GenericDrawAxes::GenericDrawAxes(int t)
    : gotFirstSpike(false), type(t)
{
    ylims[0] = 0;
    ylims[1] = 1;

    xlims[0] = 0;
    xlims[1] = 1;

    font = Font("Default", 12, Font::plain);

}

GenericDrawAxes::~GenericDrawAxes()
{

}

bool GenericDrawAxes::updateSpikeData(const SpikeObject& newSpike)
{
    if (!gotFirstSpike)
    {
        gotFirstSpike = true;
    }

    s = newSpike;
    return true;
}

void GenericDrawAxes::setYLims(double ymin, double ymax)
{

    //std::cout << "setting y limits to " << ymin << " " << ymax << std::endl;
    ylims[0] = ymin;
    ylims[1] = ymax;
}
void GenericDrawAxes::getYLims(double* min, double* max)
{
    *min = ylims[0];
    *max = ylims[1];
}
void GenericDrawAxes::setXLims(double xmin, double xmax)
{
    xlims[0] = xmin;
    xlims[1] = xmax;
}
void GenericDrawAxes::getXLims(double* min, double* max)
{
    *min = xlims[0];
    *max = xlims[1];
}


void GenericDrawAxes::setType(int t)
{
    if (t < WAVE1 || t > PROJ3x4)
    {
        std::cout<<"Invalid Axes type specified";
        return;
    }
    type = t;
}

int GenericDrawAxes::getType()
{
    return type;
}

int GenericDrawAxes::roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
    {
        return numToRound;
    }

    int remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;
    return numToRound + multiple - remainder;
}


void GenericDrawAxes::makeLabel(int val, int gain, bool convert, char* s)
{
    if (convert)
    {
        double volt = ad16ToUv(val, gain)/1000.;
        if (abs(val)>1e6)
        {
            //val = val/(1e6);
            sprintf(s, "%.2fV", volt);
        }
        else if (abs(val)>1e3)
        {
            //val = val/(1e3);
            sprintf(s, "%.2fmV", volt);
        }
        else
            sprintf(s, "%.2fuV", volt);
    }
    else
    {
        sprintf(s,"%d", (int)val);
    }
}

double GenericDrawAxes::ad16ToUv(int x, int gain)
{
    int result = (double)(x * 20e6) / (double)(gain * pow(2.0,16));
    return result;
}


// --------------------------------------------------


WaveformAxes::WaveformAxes(SpikeDetector *p,int _channel) : GenericDrawAxes(channel),
    processor(p),
	channel(_channel),
	drawGrid(true), 
    displayThresholdLevel(0.0f),  
    spikesReceivedSinceLastRedraw(0),
    spikeIndex(0),
    bufferSize(5),
    range(250.0f),
    isOverThresholdSlider(false),
    isDraggingThresholdSlider(false)
    
{
		bDragging  = false;

	isOverUnit = -1;
	isOverBox = -1;
//	 selectedUnit = -1;
//	 selectedBox = -1;
    addMouseListener(this, true);

    thresholdColour = Colours::red;

    font = Font("Small Text",10,Font::plain);

    for (int n = 0; n < bufferSize; n++)
    {
        SpikeObject so;
        generateEmptySpike(&so, 4);

        spikeBuffer.add(so);
    }
}

void WaveformAxes::setRange(float r)
{

    //std::cout << "Setting range to " << r << std::endl;

    range = r;

    repaint();
}

void WaveformAxes::plotSpike(const SpikeObject& s, Graphics& g)
{

    float h = getHeight();
	g.setColour(juce::Colour::Colour(s.color[0],s.color[1],s.color[2]));
    //compute the spatial width for each waveform sample
    float dx = getWidth()/float(spikeBuffer[0].nSamples);

    // type corresponds to channel so we need to calculate the starting
    // sample based upon which channel is getting plotted
    int offset = channel*s.nSamples; //spikeBuffer[0].nSamples * type; //
	
    int dSamples = 1;

    float x = 0.0f;

    for (int i = 0; i < s.nSamples-1; i++)
    {
        //std::cout << s.data[sampIdx] << std::endl;

        if (*s.gain != 0)
        {
            float s1 = h/2 + float(s.data[offset+i]-32768)/float(*s.gain)*1000.0f / range * h;
            float s2 =  h/2 + float(s.data[offset+i+1]-32768)/float(*s.gain)*1000.0f / range * h;

            g.drawLine(x,
                       s1,
                       x+dx,
                       s2);
        }
          x += dx;
    }

}

void WaveformAxes::drawThresholdSlider(Graphics& g)
{

    // draw display threshold (editable)
    float h = getHeight()*(0.5f - displayThresholdLevel/range);

    g.setColour(thresholdColour);
    g.drawLine(0, h, getWidth(), h);
    g.drawText(String(roundFloatToInt(displayThresholdLevel)),2,h,25,10,Justification::left, false);

    /*
    h = getHeight()*(0.5f - detectorThresholdLevel/range);
    
    g.setColour(Colours::orange);
    g.drawLine(0, h, getWidth(), h);
	*/
}


void WaveformAxes::drawWaveformGrid(Graphics& g)
{

    float h = getHeight();
    float w = getWidth();

    g.setColour(Colours::darkgrey);

    for (float y = -range/2; y < range/2; y += 25.0f)
    {
        if (y == 0)
            g.drawLine(0,h/2 + y/range*h, w, h/2+ y/range*h,2.0f);
        else
            g.drawLine(0,h/2 + y/range*h, w, h/2+ y/range*h);
    }

}


bool WaveformAxes::updateSpikeData(const SpikeObject& s)
{
    if (!gotFirstSpike)
    {
        gotFirstSpike = true;
    }

    if (spikesReceivedSinceLastRedraw < bufferSize)
    {

        SpikeObject newSpike = s;

        spikeIndex++;
        spikeIndex %= bufferSize;

        spikeBuffer.set(spikeIndex, newSpike);

        spikesReceivedSinceLastRedraw++;
        
    }

    return true;

}

bool WaveformAxes::checkThreshold(const SpikeObject& s)
{
    int sampIdx = 40*type;

    for (int i = 0; i < s.nSamples-1; i++)
    {

        if (float(s.data[sampIdx]-32768)/float(*s.gain)*1000.0f > displayThresholdLevel)
        {
            return true;
        }

        sampIdx++;
    }

    return false;

}

void WaveformAxes::clear()
{

    spikeBuffer.clear();
    spikeIndex = 0;

    for (int n = 0; n < bufferSize; n++)
    {
        SpikeObject so;
        generateEmptySpike(&so, 4);

        spikeBuffer.add(so);
    }

    repaint();
}

void WaveformAxes::mouseMove(const MouseEvent& event)
{

    // Point<int> pos = event.getPosition();

    float y = event.y;

    float h = getHeight()*(0.5f - displayThresholdLevel/range);

    // std::cout << y << " " << h << std::endl;

    if (y > h - 10.0f && y < h + 10.0f && !isOverThresholdSlider)
    {
        thresholdColour = Colours::yellow;
        //  std::cout << "Yes." << std::endl;
        repaint();
        isOverThresholdSlider = true;
        // cursorType = MouseCursor::DraggingHandCursor;
    }
    else if ((y < h - 10.0f || y > h + 10.0f) && isOverThresholdSlider)
    {
        thresholdColour = Colours::red;
        repaint();
        isOverThresholdSlider = false;
    } else 
	{
		// are we inside a box ?
		isOverUnit =0;
		isOverBox = -1;
		strOverWhere = "";
		isOverUnitBox(event.x, event.y, isOverUnit, isOverBox, strOverWhere);
	}


}

int WaveformAxes::findUnitIndexByID(int ID)
{
	for (int k=0;k<units.size();k++) 
		if (units[k].UnitID == ID)
			return k;
	return -1;
}

void WaveformAxes::mouseDown(const juce::MouseEvent& event)
{
	
	if (event.mods.isRightButtonDown())
	{
		clear();
	}


	float h = getHeight();
	float w = getWidth();
	float microsec_span = 40.0/30000.0 * 1e6;
	float microvolt_span = range/2; 
	mouseDownX = event.x/w * microsec_span ;
	mouseDownY=(h/2- event.y)/(h/2)*microvolt_span;
	if (isOverUnit > 0) 
	{
		processor->getActiveElectrode()->spikeSort->setSelectedUnitAndbox(isOverUnit, isOverBox);
		int indx = findUnitIndexByID(isOverUnit);
		jassert(indx >= 0);
		mouseOffsetX = mouseDownX - units[indx].lstBoxes[isOverBox].x;
		mouseOffsetY = mouseDownY - units[indx].lstBoxes[isOverBox].y;
	} else 
	{
		processor->getActiveElectrode()->spikeSort->setSelectedUnitAndbox(-1, -1);

	}
//	MouseUnitOffset = ScreenToMS_uV(e.X, e.Y) - new PointD(boxOnDown.x, boxOnDown.y);

    // if (isOverThresholdSlider)
    // {
    //     cursorType = MouseCursor::DraggingHandCursor;
    // }
}


void WaveformAxes::mouseUp(const MouseEvent& event)
{
	if (bDragging)
	{
		bDragging = false;
		// send a message to processor to update its internal structure?
		Electrode *e = processor->getActiveElectrode();
		e->spikeSort->updateBoxUnits( units);
	}
    // if (isOverThresholdSlider)
    // {
    //     cursorType = MouseCursor::DraggingHandCursor;
    // }
}

void WaveformAxes::mouseDrag(const MouseEvent& event)
{
	bDragging = true;
    if (isOverUnit > 0) {
		// dragging a box
		// convert position to metric coordinates.
		float h = getHeight();
		float w = getWidth();
		float microsec_span = 40.0/30000.0 * 1e6;
		float microvolt_span = range/2; 
		float x = event.x/w * microsec_span ;
		float y=(h/2- event.y)/(h/2)*microvolt_span;

		// update units position....

		for (int k=0;k<units.size();k++) 
		{
			if (units[k].getUnitID() == isOverUnit) {
				float oldx = units[k].lstBoxes[isOverBox].x;
				float oldy = units[k].lstBoxes[isOverBox].y;
				float dx = x - oldx;
				float dy = y - oldy;

				if (strOverWhere == "right")
				{
					units[k].lstBoxes[isOverBox].w = x - oldx;
				} else 
				if (strOverWhere == "left")
				{
					units[k].lstBoxes[isOverBox].w += -dx;
					units[k].lstBoxes[isOverBox].x = x;
				}  else 
				if (strOverWhere == "bottom")
				{
					units[k].lstBoxes[isOverBox].y += dy;
					units[k].lstBoxes[isOverBox].h -= dy;
				} else 
				if (strOverWhere == "top")
				{
					units[k].lstBoxes[isOverBox].h = dy;
				}   else

					if (strOverWhere == "topright")
				{
					units[k].lstBoxes[isOverBox].w = x - oldx;
					units[k].lstBoxes[isOverBox].h = dy;

				} else 
				if (strOverWhere == "topleft")
				{
					units[k].lstBoxes[isOverBox].w += -dx;
					units[k].lstBoxes[isOverBox].x = x;
					units[k].lstBoxes[isOverBox].h = dy;
				}  else 
				if (strOverWhere == "bottomright")
				{
					units[k].lstBoxes[isOverBox].y += dy;
					units[k].lstBoxes[isOverBox].h -= dy;
					units[k].lstBoxes[isOverBox].w = x - oldx;
			
				} else 
				if (strOverWhere == "bottomleft")
				{
					units[k].lstBoxes[isOverBox].w += -dx;
					units[k].lstBoxes[isOverBox].x = x;
					units[k].lstBoxes[isOverBox].y += dy;
					units[k].lstBoxes[isOverBox].h -= dy;

				}   else 
					if (strOverWhere == "inside")
				{
					units[k].lstBoxes[isOverBox].x = x-mouseOffsetX;
					units[k].lstBoxes[isOverBox].y = y-mouseOffsetY;
				}


			}

		}

		//void WaveformAxes::isOverUnitBox(float x, float y, int &UnitID, int &BoxID, String &where) 
	} else  if (isOverThresholdSlider)
    {

        float thresholdSliderPosition =  float(event.y) / float(getHeight());
		
        if (thresholdSliderPosition > 1)
            thresholdSliderPosition = 1;
        else if (thresholdSliderPosition < -1) // Modified to allow negative thresholds.
            thresholdSliderPosition =-1;


        displayThresholdLevel = (0.5f - thresholdSliderPosition) * range;
		// update processor
		processor->getActiveElectrode()->thresholds[channel] = displayThresholdLevel;
		SpikeDetectorEditor* edt = (SpikeDetectorEditor*) processor->getEditor();
		//Array<double> th(processor->getActiveElectrode()->thresholds,processor->getActiveElectrode()->numChannels);

		for (int k=0;k<processor->getActiveElectrode()->numChannels;k++)
			edt->electrodeButtons[k]->setToggleState(false,false);

			edt->electrodeButtons[channel]->setToggleState(true,false);

		edt->setThresholdValue(channel,displayThresholdLevel);

        //std::cout << "Threshold = " << thresholdLevel << std::endl;

        repaint();
    } 
}

// MouseCursor WaveAxes::getMouseCursor()
// {
//     MouseCursor c = MouseCursor(cursorType);

//     return c;
// }

void WaveformAxes::mouseExit(const MouseEvent& event)
{
    if (isOverThresholdSlider)
    {
        isOverThresholdSlider = false;
        thresholdColour = Colours::red;
        repaint();
    }
}

float WaveformAxes::getDisplayThreshold()
{
    return displayThresholdLevel;
}

void WaveformAxes::setDetectorThreshold(float t)
{
    displayThresholdLevel = t;
}



void WaveformAxes::isOverUnitBox(float x, float y, int &UnitID, int &BoxID, String &where) 
{
	setMouseCursor(MouseCursor::NormalCursor);
	
    float h = getHeight();
    float w = getWidth();
	// Map box coordinates to screen coordinates.
	// Assume time span is 40 samples at 30 Khz?
	float microsec_span = 40.0/30000.0 * 1e6;
	float microvolt_span = range/2; 

	// Typical spike is 40 samples, at 30kHz ~ 1.3 ms or 1300 usecs.
	for (int k=0;k<units.size();k++)
	{
		for (int boxiter = 0;boxiter< units[k].lstBoxes.size(); boxiter++) 
		{
			Box B = units[k].lstBoxes[boxiter];
			float rectx1 = B.x / microsec_span * w;
			float recty1 = h/2 - (B.y / microvolt_span * h/2);

			float rectx2 = (B.x+B.w) / microsec_span * w;
			float recty2 = h/2 - ((B.y+B.h) / microvolt_span * h/2);
			if (rectx1 > rectx2)
				swapVariables(rectx1,rectx2);
			if (recty1 > recty2)
				swapVariables(recty1,recty2);

			if (x >= rectx1 - 10 & y >= recty1 -10 & x <= rectx2 + 10 & y <= recty2+10)
			{
				setMouseCursor(MouseCursor::DraggingHandCursor);
				UnitID = units[k].UnitID;
				BoxID = boxiter;
				if (x >= rectx1 - 10 & x <= rectx1 + 10 && y >= recty1-10 & y <= recty1+10)
				{
					where = "topleft";
					setMouseCursor(MouseCursor::TopLeftCornerResizeCursor);
				} else
					if (x >= rectx2 - 10 & x <= rectx2 + 10 && y >= recty1-10 & y <= recty1+10)
				{
					where = "topright";
					setMouseCursor(MouseCursor::TopRightCornerResizeCursor);
				} else 
					if (x >= rectx1 - 10 & x <= rectx1 + 10 && y >= recty2-10 & y <= recty2+10)
				{
					where = "bottomleft";
					setMouseCursor(MouseCursor::BottomLeftCornerResizeCursor);
				} else 
					if (x >= rectx2 - 10 & x <= rectx2 + 10 && y >= recty2-10 & y <= recty2+10)
				{
					where = "bottomright";
					setMouseCursor(MouseCursor::BottomRightCornerResizeCursor);
				} else
				if (x >= rectx1 - 10 & x <= rectx1 + 10)
				{
					where = "left";
					setMouseCursor(MouseCursor::LeftEdgeResizeCursor);
				}
				else if (x >= rectx2 - 10 & x <= rectx2 + 10)
				{
					where = "right";
					setMouseCursor(MouseCursor::RightEdgeResizeCursor);
				}
				else if (y >= recty1 - 10 & y <= recty1 + 10)
				{
					setMouseCursor(MouseCursor::TopEdgeResizeCursor);
					where = "top";
				}
				else if (y >= recty2 - 10 & y <= recty2 + 10)
				{
					where = "bottom";
					setMouseCursor(MouseCursor::BottomEdgeResizeCursor);
				}
				else {
					setMouseCursor(MouseCursor::DraggingHandCursor);
					where = "inside";
				}

				return;
			}
		}
	}
	

}

void WaveformAxes::drawBoxes(Graphics &g)
{
	// y,and h are given in micro volts.
	// x and w and given in micro seconds.
	
    float h = getHeight();
    float w = getWidth();
	// Map box coordinates to screen coordinates.
	// Assume time span is 40 samples at 30 Khz?
	float microsec_span = 40.0/30000.0 * 1e6;
	float microvolt_span = range/2; 

	int selectedUnitID, selectedBoxID;
	processor->getActiveElectrode()->spikeSort->getSelectedUnitAndbox(selectedUnitID, selectedBoxID);

	// Typical spike is 40 samples, at 30kHz ~ 1.3 ms or 1300 usecs.
	for (int k=0;k<units.size();k++) 
	{
		g.setColour(juce::Colour::Colour(units[k].ColorRGB[0],units[k].ColorRGB[1],units[k].ColorRGB[2]));
		
		for (int boxiter = 0;boxiter < units[k].lstBoxes.size(); boxiter++) 
		{
			Box B = units[k].lstBoxes[boxiter];
			float thickness;
			if (units[k].getUnitID() == selectedUnitID && boxiter == selectedBoxID)
				thickness = 3;
			else if (units[k].getUnitID() == isOverUnit && boxiter == isOverBox)
				thickness = 2;
			else 
				thickness = 1;


			float rectx1 = B.x / microsec_span * w;
			float recty1 = h/2 - (B.y / microvolt_span * h/2);

			float rectx2 = (B.x+B.w) / microsec_span * w;
			float recty2 = h/2 - ((B.y+B.h) / microvolt_span * h/2);
			g.drawRect(rectx1,recty1,rectx2-rectx1,recty2-recty1,thickness);

		}
	}





}

void WaveformAxes::updateUnits(std::vector<BoxUnit> _units)
{
	units = _units;
}

void WaveformAxes::paint(Graphics& g)
{
    g.setColour(Colours::black);
    g.fillRect(0,0,getWidth(), getHeight());

   // int chan = 0;

    // draw the grid lines for the waveforms

    if (drawGrid)
        drawWaveformGrid(g);

    // draw the threshold line and labels
    drawThresholdSlider(g);
	drawBoxes(g);
    // if no spikes have been received then don't plot anything
    if (!gotFirstSpike)
    {
        return;
    }


     for (int spikeNum = 0; spikeNum < bufferSize; spikeNum++)
     {

         if (spikeNum != spikeIndex)
         {
             g.setColour(Colours::grey);
             plotSpike(spikeBuffer[spikeNum], g);
         }

     }

    g.setColour(Colours::white);
    plotSpike(spikeBuffer[spikeIndex], g);


    spikesReceivedSinceLastRedraw = 0;

}

// --------------------------------------------------





PCAProjectionAxes::PCAProjectionAxes(SpikeDetector *p) : processor(p),GenericDrawAxes(0), imageDim(500),
    rangeX(250), rangeY(250), spikesReceivedSinceLastRedraw(0)
{
    projectionImage = Image(Image::RGB, imageDim, imageDim, true);
	bufferSize = 600;
	pcaMin[0] = pcaMin[1] = 0;
	pcaMax[0] = pcaMax[1] = 0;

	rangeSet = false;
	inPolygonDrawingMode = false;
    clear();
	updateProcessor = false;
	isOverUnit = -1;

	rangeUpButton = new UtilityButton("+", Font("Small Text", 10, Font::plain));
	rangeUpButton->setRadius(3.0f);
	rangeUpButton->addListener(this);
	addAndMakeVisible(rangeUpButton);

  	rangeDownButton = new UtilityButton("-", Font("Small Text", 10, Font::plain));
	rangeDownButton->setRadius(3.0f);
	rangeDownButton->addListener(this);
	addAndMakeVisible(rangeDownButton);

    redrawSpikes = true;

}




void PCAProjectionAxes::resized()
{

	rangeDownButton->setBounds(10,10, 20, 15); 
	rangeUpButton->setBounds(35,10, 20, 15); 
}


void PCAProjectionAxes::setPolygonDrawingMode(bool on)
{
	if (on) {
		inPolygonDrawingMode = true;
		setMouseCursor(MouseCursor::CrosshairCursor);

	}
	else {
		inPolygonDrawingMode = false;
		setMouseCursor(MouseCursor::NormalCursor);
	}
}

void PCAProjectionAxes::updateUnits(std::vector<PCAUnit> _units)
{
	units = _units;
}

void PCAProjectionAxes::drawUnit(Graphics &g, PCAUnit unit)
{
		float w = getWidth();
		float h = getHeight();

		int selectedUnitID, selectedBoxID;
		processor->getActiveElectrode()->spikeSort->getSelectedUnitAndbox(selectedUnitID, selectedBoxID);
	g.setColour(juce::Colour::Colour(unit.ColorRGB[0],unit.ColorRGB[1],unit.ColorRGB[2]));
	if (unit.poly.pts.size() > 2) {	
		float thickness;
		if (unit.getUnitID() == selectedUnitID)
			thickness = 3;
		else if (unit.getUnitID() == isOverUnit)
			thickness = 2;
		else 
			thickness = 1;

		for (int k=0;k<unit.poly.pts.size()-1;k++)
		{
			// convert projection coordinates to screen coordinates.
			float x1 = (unit.poly.offset.X + unit.poly.pts[k].X - pcaMin[0]) / (pcaMax[0]-pcaMin[0]) * w;
			float y1 = (unit.poly.offset.Y + unit.poly.pts[k].Y - pcaMin[1]) / (pcaMax[1]-pcaMin[1]) * h;
			float x2 = (unit.poly.offset.X + unit.poly.pts[k+1].X - pcaMin[0]) / (pcaMax[0]-pcaMin[0]) * w;
			float y2 = (unit.poly.offset.Y + unit.poly.pts[k+1].Y - pcaMin[1]) / (pcaMax[1]-pcaMin[1]) * h;
			g.drawLine(x1,y1,x2,y2,thickness);
		}
		float x1 = (unit.poly.offset.X + unit.poly.pts[0].X - pcaMin[0]) / (pcaMax[0]-pcaMin[0]) * w;
		float y1 = (unit.poly.offset.Y + unit.poly.pts[0].Y - pcaMin[1]) / (pcaMax[1]-pcaMin[1]) * h;
		float x2 = (unit.poly.offset.X + unit.poly.pts[unit.poly.pts.size()-1].X - pcaMin[0]) / (pcaMax[0]-pcaMin[0]) * w;
		float y2 = (unit.poly.offset.Y + unit.poly.pts[unit.poly.pts.size()-1].Y - pcaMin[1]) / (pcaMax[1]-pcaMin[1]) * h;
		g.drawLine(x1,y1,x2,y2,thickness);

	}

}
void PCAProjectionAxes::paint(Graphics& g)
{

	spikesReceivedSinceLastRedraw = 0;
	
    g.drawImage(projectionImage,
                0, 0, getWidth(), getHeight(),
                0, 0, rangeX, rangeY);


	// draw pca units polygons
	for (int k=0;k<units.size();k++)
	{
		drawUnit(g, units[k]);
	}


	if (inPolygonDrawingMode)
	{
		setMouseCursor(MouseCursor::CrosshairCursor);
		// draw polygon
		bool first = true;
		PointD prev;
			
		if (drawnPolygon.size() > 0)
		{
			g.setColour(juce::Colour::Colour(drawnUnit.ColorRGB[0],drawnUnit.ColorRGB[1],drawnUnit.ColorRGB[2]));
		
		for (std::list<PointD>::iterator it = drawnPolygon.begin(); it != drawnPolygon.end(); it++)
		{
			if (first) {
				first = false;
			} else 
			{
				g.drawLine( (*it).X, (*it).Y, prev.X,prev.Y);
			}
			prev = *it;
		}
		
		g.drawLine(drawnPolygon.front().X,drawnPolygon.front().Y,drawnPolygon.back().X,drawnPolygon.back().Y);
		}
	}
    
    //Graphics im(projectionImage);

	if (redrawSpikes)
	{
		// recompute image
		int w = getWidth();
		int h = getHeight();
		projectionImage.clear(juce::Rectangle<int>(0, 0, projectionImage.getWidth(), projectionImage.getHeight()),
			Colours::black);

		bool subsample = false;
		int dk = (subsample) ? 5 : 1;

		for (int k=0;k<bufferSize;k+=dk)
		{
			drawProjectedSpike(spikeBuffer[k]);
		}
		redrawSpikes = false;
	}

}


void PCAProjectionAxes::drawProjectedSpike(SpikeObject s)
{
	if (rangeSet) 
	{
		Graphics g(projectionImage);
        
		g.setColour(juce::Colour::Colour(s.color[0],s.color[1],s.color[2]));
		
		float x = (s.pcProj[0] - pcaMin[0]) / (pcaMax[0]-pcaMin[0]) * rangeX;
		float y = (s.pcProj[1] - pcaMin[1]) / (pcaMax[1]-pcaMin[1]) * rangeY;
		if (x >= 0 & y >= 0 & x <= rangeX & y <= rangeY)
			g.fillEllipse(x,y,2,2);
	}
}

void PCAProjectionAxes::redraw(bool subsample)
{
//	Graphics g(projectionImage);
//
//	// recompute image
//	int w = getWidth();
//	int h = getHeight();
//	projectionImage.clear(juce::Rectangle<int>(0, 0, projectionImage.getWidth(), projectionImage.getHeight()),
//                          Colours::black);
//
//	int dk = (subsample) ? 5 : 1;
//
//	for (int k=0;k<bufferSize;k+=dk)
//	{
//		drawProjectedSpike(spikeBuffer[k]);
//	}
	
}

void PCAProjectionAxes::setPCARange(float p1min, float p2min, float p1max, float p2max)
{
	
	pcaMin[0] = p1min;
	pcaMin[1] = p2min;
	pcaMax[0] = p1max;
	pcaMax[1] = p2max;
	rangeSet = true;
    
    redrawSpikes = true;

}

bool PCAProjectionAxes::updateSpikeData(const SpikeObject& s)
{
   
    if (spikesReceivedSinceLastRedraw < bufferSize)
    {

        SpikeObject newSpike = s;

        spikeIndex++;
        spikeIndex %= bufferSize;

        spikeBuffer.set(spikeIndex, newSpike);

        spikesReceivedSinceLastRedraw++;
        //drawProjectedSpike(newSpike);
        redrawSpikes = true;
		
    }
    return true;
}


void PCAProjectionAxes::clear()
{
    projectionImage.clear(juce::Rectangle<int>(0, 0, projectionImage.getWidth(), projectionImage.getHeight()),
                          Colours::black);


    spikeBuffer.clear();
    spikeIndex = 0;

    redrawSpikes = true;
    //repaint();
}


void PCAProjectionAxes::mouseDrag(const juce::MouseEvent& event)
{

	if (!inPolygonDrawingMode)
	{

		setMouseCursor(MouseCursor::DraggingHandCursor);
		int selectedUnitID, selectedBoxID;
		processor->getActiveElectrode()->spikeSort->getSelectedUnitAndbox(selectedUnitID, selectedBoxID);

		if (isOverUnit > 0 && selectedUnitID == isOverUnit) {
			// pan unit
			int unitindex=-1;

			for (int k=0;k<units.size();k++) {
				if (units[k].getUnitID() == selectedUnitID)
				{
					unitindex = k;
					break;
				}
			}
		jassert(unitindex >= 0);

			int w = getWidth();
			int h = getHeight();
			float range0 = pcaMax[0]-pcaMin[0];
			float range1 = pcaMax[1]-pcaMin[1];

			float dx = float(event.x-prevx) / w*range0;
			float dy = float(event.y-prevy) / h*range1;

			
			units[unitindex].poly.offset.X += dx;
			units[unitindex].poly.offset.Y += dy;
			updateProcessor = true;
			// draw polygon
			prevx = event.x;
			prevy = event.y;
			
		} else 
		{
			// Pan PCA space
			int w = getWidth();
			int h = getHeight();
			float range0 = pcaMax[0]-pcaMin[0];
			float range1 = pcaMax[1]-pcaMin[1];

			float dx = -float(event.x-prevx) / w*range0;
			float dy = -float(event.y-prevy) / h*range1;

			pcaMin[0]+=dx;
			pcaMin[1]+=dy;
			pcaMax[0]+=dx;
			pcaMax[1]+=dy;

			// draw polygon
			prevx = event.x;
			prevy = event.y;
            
            redrawSpikes = true;
		}
	
	} else 
	{
		int pixel_quantizer = 6;
		float distance = float(event.x-prevx)*float(event.x-prevx)+
						 float(event.y-prevy)*float(event.y-prevy);
		if (distance > pixel_quantizer*pixel_quantizer) {// add a point every n pixels.
			drawnPolygon.push_back(PointD(event.x,event.y));
		// draw polygon
	prevx = event.x;
	prevy = event.y;

		repaint();	
		}
	}

}

void PCAProjectionAxes::mouseUp(const juce::MouseEvent& event)
{
    repaint();
	//redraw(false);
	setMouseCursor(MouseCursor::NormalCursor);
	if 	(updateProcessor)
	{
		processor->getActiveElectrode()->spikeSort->updatePCAUnits(units);
			updateProcessor = false;
			
	}
		
	if (inPolygonDrawingMode)
	{
		inPolygonDrawingMode = false;
		SpikeDetectorEditor* edt = (SpikeDetectorEditor*)processor->getEditor();
		edt->spikeDetectorCanvas->addPolygonUnitButton->setToggleState(false,false);

		// convert pixel coordinates to pca space coordinates and update unit
		cPolygon poly;
		poly.pts.resize(drawnPolygon.size());
		int k=0;

		float w = getWidth();
		float h = getHeight();
 		float range0 = pcaMax[0]-pcaMin[0];
		float range1 = pcaMax[1]-pcaMin[1];
	
		for (std::list<PointD>::iterator it = drawnPolygon.begin();it != drawnPolygon.end();it++,k++)
		{
			poly.pts[k].X = (*it).X / w * range0 + pcaMin[0];
			poly.pts[k].Y = (*it).Y / h * range1 + pcaMin[1];
		}
		drawnUnit.poly = poly;
		units.push_back(drawnUnit);
		// add a new PCA unit
		Electrode *e = processor->getActiveElectrode();
		e->spikeSort->addPCAunit(drawnUnit);


		uint8 r,g,b;
		e->spikeSort->getUnitColor(drawnUnit.getUnitID(), r,g,b);
		
		String eventlog = "NewUnit "+String(e->electrodeID) + " "+String(drawnUnit.getUnitID()) +" "+String(r)+" "+String(g)+" "+String(b);
		processor->addNetworkEventToQueue(StringTS(eventlog));

		drawnPolygon.clear();
	}
}


void PCAProjectionAxes::mouseMove(const juce::MouseEvent& event)
{
	isOverUnit = -1;
	float w = getWidth();
	float h = getHeight();

	for (int k=0;k<units.size();k++){
				// convert projection coordinates to screen coordinates.
			float x1 = ((float)event.x/w) * (pcaMax[0]-pcaMin[0]) +  pcaMin[0];
			float y1 = ((float)event.y/h) * (pcaMax[1]-pcaMin[1]) +  pcaMin[1];
		if (units[k].isPointInsidePolygon(PointD(x1,y1))) {
			isOverUnit = units[k].getUnitID();
			break;
		}

	}


}


void PCAProjectionAxes::mouseDown(const juce::MouseEvent& event)
{
	prevx = event.x;
	prevy = event.y;
	if (event.mods.isRightButtonDown())
	{
		clear();
	}
	if (inPolygonDrawingMode)
	{
		drawnUnit = PCAUnit(processor->getActiveElectrode()->spikeSort->generateUnitID());
		drawnPolygon.push_back(PointD(event.x,event.y));
	} else {
		if (isOverUnit > 0)
				processor->getActiveElectrode()->spikeSort->setSelectedUnitAndbox(isOverUnit, -1);
		else
			processor->getActiveElectrode()->spikeSort->setSelectedUnitAndbox(-1, -1);
	}
}


bool PCAProjectionAxes::keyPressed(const KeyPress& key)
{
	KeyPress e = KeyPress::createFromDescription("escape");
	
    if (key.isKeyCode(e.getKeyCode()) && inPolygonDrawingMode) // C
    {
		inPolygonDrawingMode = false;
		setMouseCursor(MouseCursor::NormalCursor);
        return true;
    }
	return false;
}

void PCAProjectionAxes::buttonClicked(Button* button)
{
	    if (button == rangeDownButton)
		{
			float range0 = pcaMax[0]-pcaMin[0];
			float range1 = pcaMax[1]-pcaMin[1];
			pcaMin[0] = pcaMin[0] - 0.1 * range0;
			pcaMax[0] = pcaMax[0] + 0.1 * range0;
			pcaMin[1] = pcaMin[1] - 0.1 * range1;
			pcaMax[1] = pcaMax[1] + 0.1 * range1;
			setPCARange(pcaMin[0], pcaMin[1], pcaMax[0], pcaMax[1]);
		} 

		else if  (button == rangeUpButton)
		{
			float range0 = pcaMax[0]-pcaMin[0];
			float range1 = pcaMax[1]-pcaMin[1];
			pcaMin[0] = pcaMin[0] + 0.1 * range0;
			pcaMax[0] = pcaMax[0] - 0.1 * range0;
			pcaMin[1] = pcaMin[1] + 0.1 * range1;
			pcaMax[1] = pcaMax[1] - 0.1 * range1;

			setPCARange(pcaMin[0], pcaMin[1], pcaMax[0], pcaMax[1]);
		}

}