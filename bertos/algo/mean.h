#warning revise me!


/**
 *  DECLARE_SMEAN(temperature, uint8_t, uint16_t);
 *  for (i = 0; i < TEMP_MEANS; ++i)
 *    SMEAN_ADD(temperature, adc_get(), TEMP_MEANS);
 *  printf("mean temperature = %d\n", SMEAN_GET(temperature));
 */

/**
 * Instantiate a mean instance
 */
#define DECLARE_SMEAN(name, Type, SumType) \
	struct { \
		SumType sum; \
		Type result; \
		int count; \
	} name = { 0, 0, 0 }

/**
 * Insert a new sample into the mean.
 *
 * \note \a mean and \a max_samples are evaluated multiple times
 */
#define SMEAN_ADD(mean, sample, max_samples) \
	do { \
		(mean).sum += (sample); \
		if ((mean).count++ >= (max_samples)) \
		{ \
			(mean).result = (mean).sum / (max_samples); \
			(mean).sum = 0; \
			(mean).count = 0; \
		} \
	} while (0)

/**
 * Return current mean value.
 */
#define SMEAN_GET(mean)  ((mean).result)

