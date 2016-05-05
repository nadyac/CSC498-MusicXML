<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:my-ns="http://my.com" xmlns:my-ns2="http://my.com2"
	exclude-result-prefixes="my-ns my-ns2">

	<xsl:output encoding="UTF-8" indent="yes"/>

	<xsl:param name="p1"/>
	<xsl:param name="p2"/>
	<xsl:param name="p3"/>

	<xsl:param name="my-ns:param"/>
	<xsl:param name="my-ns2:param2"/>

	<xsl:template match="/">

		<results>
			<p1><xsl:value-of select="$p1"/></p1>
			<p2><xsl:value-of select="$p2"/></p2>
			<p3><xsl:value-of select="$p3"/></p3>
			<xsl:for-each select="//*[name()=$p1 or name()=$p2 or name()=$p3]">
				<xsl:element name="{name()}"/>
			</xsl:for-each>
		</results>

	</xsl:template>

	<xsl:template match="/" mode="foo">
		<mode>This is 'foo' mode.</mode>
	</xsl:template>

	<xsl:template match="/" mode="my-ns:foo">
		<my-ns:param><xsl:value-of select="$my-ns:param"/></my-ns:param>
		<my-ns2:param2><xsl:value-of select="$my-ns2:param2"/></my-ns2:param2>
		<mode>This is 'my-ns:foo' mode.</mode>
	</xsl:template>

</xsl:stylesheet>