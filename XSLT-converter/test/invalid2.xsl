<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="/">

		<root>
			<xsl:text>&#xA;</xsl:text>
			<xsl:element name="{concat(unk, '=name')}"/>
		</root>

	</xsl:template>

</xsl:stylesheet>