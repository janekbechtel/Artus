#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_list.hpp>

#include "EventPipeline.h"
#include "EventProviderBase.h"

/*
 * The EventPipelineRunner utilizes a user-provided EventProvider to load events and passes them to
 * all registered EventPipelines.
 * Furthermore, GlobalEventProducers can be registered, which can generate Pipeline-Independet meta data
 * of the event. This GlobalEventProducers are run before any pipeline is started and the generated data is passed to the
 * pipelines.
 */

template<class TPipeline, class TGlobalMetaProducer>
class EventPipelineRunner: public boost::noncopyable {
public:
	typedef boost::ptr_list<TPipeline> Pipelines;
	typedef typename Pipelines::iterator PipelinesIterator;

	typedef boost::ptr_list<TGlobalMetaProducer> GlobalMetaProducer;
	typedef typename GlobalMetaProducer::iterator GlobalMetaProducerIterator;

	/*
	 * Add a pipeline. The object is destroy in the destructor of the EventPipelineRunner
	 */
	void AddPipeline(TPipeline* pline) {
		m_pipelines.push_back(pline);
	}

	/*
	 * Add a GlobalMetaProducer. The object is destroy in the destructor of the EventPipelineRunner
	 */
	void AddGlobalMetaProducer(TGlobalMetaProducer* metaProd) {
		m_globalMetaProducer.push_back(metaProd);
	}

	/*
	 * Add a range of pipelines. The object is destroy in the destructor of the EventPipelineRunner
	 */
	void AddPipelines(std::vector<TPipeline*> pVec) {

		for (auto * it : pVec) {
			AddPipeline(it);
		}
	}

	/*
	 * Run the GlobalMetaProducers and all pipelines.
	 * give any pipeline setting here: only the global meta data producer will
	 * read from the global settings ...
	 *
	 */
	template<  class TEvent, class TMetaData, class TSettings>
	void RunPipelines(EventProviderBase<TEvent>& evtProvider,
			TSettings const& settings) {
		long long firstEvent = 0; // settings.Global()->GetSkipEvents();
		long long nEvents = evtProvider.GetOverallEventCount();
		/*if (settings.Global()->GetEventCount() >= 0)
		 nEvents = firstEvent + settings.Global()->GetEventCount();
		 if (firstEvent != 0 || nEvents != evtProvider.GetOverallEventCount())
		 CALIB_LOG(red << "Warning: Custom range of events: " << firstEvent << " to " << nEvents << reset)
		 */
		bool bEventValid = true;

		// init global producers
		for (GlobalMetaProducerIterator it = m_globalMetaProducer.begin();
				it != m_globalMetaProducer.end(); it++) {
			it->Init(settings);
		}

		for (long long i = firstEvent; i < nEvents; ++i) {
			// TODO refactor the evtProvider to clean up this mess with the hltTools
			evtProvider.GotoEvent(i);
			TMetaData metaDataGlobal;
			//metaDataGlobal.m_hltInfo = hltTools;

			// create global meta data
			for (GlobalMetaProducerIterator it = m_globalMetaProducer.begin();
					it != m_globalMetaProducer.end(); it++) {
				bEventValid = it->PopulateGlobalMetaData(
						evtProvider.GetCurrentEvent(), metaDataGlobal,
						settings);
				//CALIB_LOG(it->GetContent())
				if (!bEventValid)
					break;
			}

			// run the pipelines, if the event is valid
			if (bEventValid) {
				for (PipelinesIterator it = m_pipelines.begin();
						it != m_pipelines.end(); it++) {
					if (it->GetSettings().GetLevel() == 1)
						it->RunEvent(evtProvider.GetCurrentEvent(),
								metaDataGlobal);
				}
			}
			metaDataGlobal.ClearContent();
		}

		// first safe the results ( > plots ) from all level one pipelines
		for (PipelinesIterator it = m_pipelines.begin();
				!(it == m_pipelines.end()); it++) {
			if (it->GetSettings().GetLevel() == 1)
				it->FinishPipeline();
		}

		// run the pipelines greater level one
		bool noPipelineRun = false;
		size_t curLevel = 2;
		while (!noPipelineRun) {
			noPipelineRun = true;
			for (PipelinesIterator it = m_pipelines.begin();
					it != m_pipelines.end(); it++) {
				if (it->GetSettings().GetLevel() == curLevel) {
					noPipelineRun = false;

					it->Run();
					it->FinishPipeline();
				}
			}
			curLevel++;
		}
	}

private:

	Pipelines m_pipelines;
	GlobalMetaProducer m_globalMetaProducer;
};
